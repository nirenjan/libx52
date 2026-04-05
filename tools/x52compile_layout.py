#!/usr/bin/env python3
# Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
"""Keyboard layout source (``.layout``) compiler and HID token maps.

Compiles human-readable ``.layout`` files into v1 ``.x52l`` binaries (see
``daemon/layout_format.h``). The output file basename must be
``{name}.x52l`` with ``name`` taken from the ``name:`` metadata (after any
``--truncate-metadata`` trimming), matching ``Profiles.KeyboardLayout``.
The non-modifier key set matches
``x52_layout_usage_key_allowed`` in ``daemon/layout_usage_allowlist.c`` and
``vkm_key`` / ``vkm_key_modifiers`` in ``vkm/vkm.h``.
"""

import argparse
import os
import re
import struct
import sys
import zlib
from typing import Dict, List, Optional, Tuple

# ---------------------------------------------------------------------------
# HID page 0x07 usages allowed as the layout RHS key (one byte, non-modifier).
# Main block: 0x04 (A) through 0x39 (Caps Lock); plus ISO third row backslash (0x64).
# ---------------------------------------------------------------------------

ALLOWED_KEY_USAGES = frozenset(range(0x04, 0x3A)) | frozenset((0x64,))


def _letter_usage(ch: str) -> int:
    o = ord(ch)
    if not (ord("A") <= o <= ord("Z") or ord("a") <= o <= ord("z")):
        raise ValueError(ch)
    return 0x04 + (ord(ch.upper()) - ord("A"))


def _build_key_token_map() -> Dict[str, int]:
    m: Dict[str, int] = {}

    for c in "ABCDEFGHIJKLMNOPQRSTUVWXYZ":
        u = _letter_usage(c)
        m[c] = u
        m[c.lower()] = u

    digit_usages = [0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27]
    for d, u in zip("1234567890", digit_usages):
        m[d] = u

    named = {
        "ENTER": 0x28,
        "ESCAPE": 0x29,
        "ESC": 0x29,
        "BACKSPACE": 0x2A,
        "TAB": 0x2B,
        "SPACE": 0x2C,
        "MINUS": 0x2D,
        "EQUAL": 0x2E,
        "LEFT_BRACKET": 0x2F,
        "RIGHT_BRACKET": 0x30,
        "BACKSLASH": 0x31,
        "NONUS_HASH": 0x32,
        "SEMICOLON": 0x33,
        "APOSTROPHE": 0x34,
        "GRAVE_ACCENT": 0x35,
        "GRAVE": 0x35,
        "COMMA": 0x36,
        "PERIOD": 0x37,
        "SLASH": 0x38,
        "CAPS_LOCK": 0x39,
        "CAPS": 0x39,
        "INTL_BACKSLASH": 0x64,
    }
    m.update(named)

    bad = {k: v for k, v in m.items() if v not in ALLOWED_KEY_USAGES}
    if bad:
        raise RuntimeError("internal error: token maps to disallowed usage: %r" % bad)
    return m


KEY_TOKEN_TO_USAGE: Dict[str, int] = _build_key_token_map()

# Matches vkm_key_modifiers in vkm/vkm.h (HID modifier byte bits).
MODIFIER_TOKEN_TO_BIT: Dict[str, int] = {
    "LCTRL": 1 << 0,
    "LSHIFT": 1 << 1,
    "LALT": 1 << 2,
    "LGUI": 1 << 3,
    "RCTRL": 1 << 4,
    "RSHIFT": 1 << 5,
    "RALT": 1 << 6,
    "RGUI": 1 << 7,
    # Convenience aliases — same as VKM_KEY_MOD_* macros in vkm.h
    "CTRL": 1 << 0,
    "SHIFT": 1 << 1,
    "ALT": 1 << 2,
    "GUI": 1 << 3,
}

_MOD_KNOWN_BITS = 0
for _b in MODIFIER_TOKEN_TO_BIT.values():
    _MOD_KNOWN_BITS |= _b
if _MOD_KNOWN_BITS != 0xFF:
    raise RuntimeError("internal error: modifier map must cover HID bits 0-7")


def modifier_mask_from_tokens(tokens: str) -> int:
    """OR together modifier bits for plus-separated tokens (e.g. ``'CTRL+SHIFT'``)."""
    mask = 0
    for raw in tokens.replace(" ", "").split("+"):
        if not raw:
            continue
        try:
            bit = MODIFIER_TOKEN_TO_BIT[raw.upper()]
        except KeyError as e:
            raise KeyError("unknown modifier token %r" % raw) from e
        mask |= bit
    return mask


def usage_from_key_token(token: str) -> int:
    """Resolve a single key token string to a HID usage (page 0x07)."""
    key = token.strip()
    if key.isalpha() and len(key) == 1:
        u = KEY_TOKEN_TO_USAGE[key]
    else:
        u = KEY_TOKEN_TO_USAGE[key.upper()]
    return u


class LayoutCompileError(SyntaxError):
    """Invalid layout source or oversize metadata.

    Subclasses :class:`SyntaxError` so errors can carry ``filename`` and ``lineno``
    and render as ``message (path, line N)``. Pass ``path`` and ``lineno`` only
    when ``message`` is the detail alone; for whole-file problems pass one fully
    formatted string and leave location arguments unset.
    """

    def __init__(self, message, path=None, lineno=None):
        if path is not None and lineno is not None:
            SyntaxError.__init__(self, message, (path, lineno, None, None))
        else:
            SyntaxError.__init__(self, message)


HEADER_BYTES = 128
FORMAT_VERSION = 1
CODEPOINT_LIMIT_MAX = 0x110000
NAME_MAX_UTF8 = 31
DESC_MAX_UTF8 = 63

# v1 header flags (big-endian on disk); must match daemon/layout_format.h
FLAG_NAME_TRUNCATED = 1
FLAG_DESCRIPTION_TRUNCATED = 2
FLAGS_KNOWN_V1 = FLAG_NAME_TRUNCATED | FLAG_DESCRIPTION_TRUNCATED

# Fixed 128-byte header: magic, version, flags, codepoint_limit, checksum (BE).
_HEADER_PREFIX = struct.Struct("!4sHHII")

# Layout basename / ``name:`` field: matches safe config tokens (see Profiles.KeyboardLayout).
_LAYOUT_NAME_RE = re.compile(r"^[A-Za-z0-9_-]+$")


def _validate_layout_name(name: str, path: str) -> None:
    if not _LAYOUT_NAME_RE.fullmatch(name):
        raise LayoutCompileError(
            "%s: layout name must contain only ASCII letters, digits, hyphen, or underscore"
            % path
        )


def _utf8_trim_to_max_bytes(text: str, max_bytes: int) -> Tuple[str, bool]:
    """Return ``(s, truncated)`` with ``len(s.encode('utf-8')) <= max_bytes``."""
    raw = text.encode("utf-8")
    if len(raw) <= max_bytes:
        return text, False
    cut = raw[:max_bytes]
    while cut:
        try:
            return cut.decode("utf-8"), True
        except UnicodeDecodeError:
            cut = cut[:-1]
    return "", True


def parse_chord(
    chord: str, path: Optional[str] = None, lineno: Optional[int] = None
) -> Tuple[int, int]:
    """Return ``(modifiers_byte, usage)`` for a plus-separated HID chord.

    If ``path`` and ``lineno`` are given, :exc:`LayoutCompileError` includes that
    location (via :class:`SyntaxError`); omit them for programmatic chord checks.
    """
    parts = [p.strip() for p in chord.replace(" ", "").split("+") if p.strip()]
    if not parts:
        raise LayoutCompileError("empty chord", path, lineno)
    key_tok = parts[-1]
    mod_parts = parts[:-1]
    try:
        mod = modifier_mask_from_tokens("+".join(mod_parts)) if mod_parts else 0
    except KeyError as e:
        raise LayoutCompileError("unknown modifier in %r" % chord, path, lineno) from e
    try:
        usage = usage_from_key_token(key_tok)
    except KeyError as e:
        raise LayoutCompileError("unknown key token in %r" % chord, path, lineno) from e
    if usage not in ALLOWED_KEY_USAGES:
        raise LayoutCompileError("disallowed key usage for %r" % chord, path, lineno)
    return mod & 0xFF, usage & 0xFF


_U_LINE = re.compile(r"^[Uu]\+([0-9A-Fa-f]{1,6})\s+(.+)$")
_META_NAME = re.compile(r"(?i)^name:\s*(.*)$")
_META_DESC = re.compile(r"(?i)^description:\s*(.*)$")


def _parse_layout_source(
    path: str, text: str
) -> Tuple[str, str, List[Tuple[int, int, str]]]:
    """First pass: metadata and mapping lines. Returns ``(name, description, raw_maps)``."""
    name = ""  # type: str
    description = ""  # type: str
    raw_maps = []  # type: List[Tuple[int, int, str]]

    for lineno, line in enumerate(text.splitlines(), 1):
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        mn = _META_NAME.match(stripped)
        if mn:
            name = mn.group(1).strip()
            continue
        md = _META_DESC.match(stripped)
        if md:
            description = md.group(1).strip()
            continue
        cp, chord = _parse_mapping_line(path, lineno, stripped)
        raw_maps.append((cp, lineno, chord))

    if not name:
        raise LayoutCompileError("%s: missing name: metadata" % path)

    return name, description, raw_maps


def _layout_stored_name_from_bytes(buf: bytes) -> str:
    """UTF-8 layout name as stored in the v1 header field (offset 16, up to first NUL)."""
    end = buf.index(0, 16, 48)
    return buf[16:end].decode("utf-8")


def _parse_mapping_line(path: str, lineno: int, line: str) -> Tuple[int, str]:
    s = line.strip()
    m = _U_LINE.match(s)
    if m:
        cp = int(m.group(1), 16)
        if cp < 0 or cp > 0x10FFFF or (cp >= 0xD800 and cp <= 0xDFFF):
            raise LayoutCompileError("invalid Unicode scalar", path, lineno)
        return cp, m.group(2).strip()
    if not s:
        raise LayoutCompileError("empty line", path, lineno)
    ch = s[0]
    rest = s[1:].lstrip()
    if not rest:
        raise LayoutCompileError("missing chord after character", path, lineno)
    return ord(ch), rest


def compile_layout_source(path: str, text: str, *, truncate_metadata: bool = False) -> bytes:
    """Parse ``.layout`` source and return ``.x52l`` bytes.

    ``codepoint_limit`` is ``max_mapped_scalar + 1`` (dense table size).

    If ``truncate_metadata`` is false (default), oversize ``name:`` / ``description:``
    values are errors. If true, UTF-8 strings are trimmed to the field limits
    (no split codepoints) and ``FLAG_NAME_TRUNCATED`` / ``FLAG_DESCRIPTION_TRUNCATED``
    are set in the on-disk ``flags`` field.
    """
    name, description, raw_maps = _parse_layout_source(path, text)

    flags = 0
    if truncate_metadata:
        name, name_t = _utf8_trim_to_max_bytes(name, NAME_MAX_UTF8)
        if name_t:
            flags |= FLAG_NAME_TRUNCATED
        if not name:
            raise LayoutCompileError("%s: name: empty after truncation to metadata limit" % path)
        description, desc_t = _utf8_trim_to_max_bytes(description, DESC_MAX_UTF8)
        if desc_t:
            flags |= FLAG_DESCRIPTION_TRUNCATED
    else:
        name_b = name.encode("utf-8")
        if len(name_b) > NAME_MAX_UTF8:
            raise LayoutCompileError(
                "%s: layout name exceeds %d UTF-8 bytes" % (path, NAME_MAX_UTF8)
            )
        desc_b = description.encode("utf-8")
        if len(desc_b) > DESC_MAX_UTF8:
            raise LayoutCompileError(
                "%s: description exceeds %d UTF-8 bytes" % (path, DESC_MAX_UTF8)
            )

    _validate_layout_name(name, path)

    name_b = name.encode("utf-8")
    desc_b = description.encode("utf-8")
    assert len(name_b) <= NAME_MAX_UTF8
    assert len(desc_b) <= DESC_MAX_UTF8
    if flags & ~FLAGS_KNOWN_V1:
        raise RuntimeError("internal error: unknown layout flags bits")

    if not raw_maps:
        raise LayoutCompileError("%s: no mappings" % path)

    by_cp = {}  # type: Dict[int, Tuple[int, int, int]]
    for cp, lineno, chord in raw_maps:
        if cp in by_cp:
            raise LayoutCompileError(
                "duplicate mapping for U+%04X (also at line %d)" % (cp, by_cp[cp][2]),
                path,
                lineno,
            )
        mod, usage = parse_chord(chord, path, lineno)
        if usage == 0:
            raise LayoutCompileError("chord must not resolve to usage 0", path, lineno)
        by_cp[cp] = (mod, usage, lineno)

    max_cp = max(by_cp.keys())
    limit = max_cp + 1
    if limit > CODEPOINT_LIMIT_MAX:
        raise LayoutCompileError("%s: codepoint_limit would exceed 0x110000" % path)

    body = limit * 2
    buf = bytearray(HEADER_BYTES + body)

    _HEADER_PREFIX.pack_into(buf, 0, b"X52L", FORMAT_VERSION, flags, limit, 0)
    buf[16 : 16 + len(name_b)] = name_b
    buf[48 : 48 + len(desc_b)] = desc_b

    base = HEADER_BYTES
    for cp, (mod, usage, _) in by_cp.items():
        off = base + 2 * cp
        buf[off] = mod
        buf[off + 1] = usage

    crc_input = bytes(buf[0:12]) + b"\0\0\0\0" + bytes(buf[16:])
    crc = zlib.crc32(crc_input, 0) & 0xFFFFFFFF
    struct.pack_into("!I", buf, 12, crc)
    return bytes(buf)


def compile_layout_file(
    src_path: str, dst_path: str, *, truncate_metadata: bool = False
) -> None:
    """Compile ``src_path`` to ``dst_path``.

    The destination basename must be ``{name}.x52l`` where ``name`` is the layout name
    stored in the binary (from the ``name:`` line, after any ``--truncate-metadata``
    trimming) so it matches ``Profiles.KeyboardLayout``.
    """
    with open(src_path, "r", encoding="utf-8-sig") as f:
        text = f.read()
    out = compile_layout_source(src_path, text, truncate_metadata=truncate_metadata)
    stored_name = _layout_stored_name_from_bytes(out)
    expected_base = stored_name + ".x52l"
    if os.path.basename(dst_path) != expected_base:
        raise LayoutCompileError(
            "%s: output file must be named %r (from layout name:), got %r"
            % (src_path, expected_base, os.path.basename(dst_path))
        )
    with open(dst_path, "wb") as f:
        f.write(out)


def main(argv: Optional[List[str]] = None) -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Compile human-readable .layout source into a v1 .x52l binary "
            "(see daemon/layout_format.h)."
        )
    )
    parser.add_argument(
        "--truncate-metadata",
        action="store_true",
        help=(
            "Trim oversize name:/description: to header field limits and set truncation flags."
        ),
    )
    parser.add_argument(
        "source",
        metavar="layout.source",
        help="Input .layout file.",
    )
    parser.add_argument(
        "output",
        nargs="?",
        metavar="out.x52l",
        help=(
            "Output path; basename must be {name}.x52l from layout metadata. "
            "If omitted, write ./{name}.x52l in the current directory."
        ),
    )
    parsed = parser.parse_args(argv)

    truncate_metadata = bool(parsed.truncate_metadata)
    src_path = parsed.source

    try:
        if parsed.output is None:
            with open(src_path, "r", encoding="utf-8-sig") as f:
                text = f.read()
            out = compile_layout_source(
                src_path, text, truncate_metadata=truncate_metadata
            )
            stored_name = _layout_stored_name_from_bytes(out)
            dst_path = os.path.join(os.getcwd(), stored_name + ".x52l")
            with open(dst_path, "wb") as f:
                f.write(out)
        else:
            compile_layout_file(
                src_path, parsed.output, truncate_metadata=truncate_metadata
            )
    except LayoutCompileError as e:
        sys.stderr.write("%s\n" % e)
        return 1
    except OSError as e:
        sys.stderr.write("%s\n" % e)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
