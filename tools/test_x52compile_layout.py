#!/usr/bin/env python3
# Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
"""TAP tests for ``tools/x52compile_layout.py`` (v1 ``.x52l`` compiler)."""

import pathlib
import struct
import subprocess
import sys
import tempfile
import zlib


def _emit_tap(test_cases):
    """test_cases: iterable of (description, callable). Callable raises on failure."""
    case_list = list(test_cases)
    print("TAP version 13")
    print("1..%d" % len(case_list))
    failed = 0
    for i, (desc, fn) in enumerate(case_list, 1):
        try:
            fn()
        except AssertionError as e:
            failed += 1
            print("not ok %d - %s" % (i, desc))
            msg = str(e) if str(e) else "assertion failed"
            for line in msg.splitlines():
                print("# %s" % line)
        else:
            print("ok %d - %s" % (i, desc))
    return 1 if failed else 0


def _x52_crc32_verify(buf):
    """CRC-32 over file with checksum field (bytes 12..15) taken as zero."""
    return zlib.crc32(buf[:12] + b"\0\0\0\0" + buf[16:]) & 0xFFFFFFFF


def main():
    root = pathlib.Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(root / "tools"))

    import x52compile_layout as x

    def check_layout_compile_error_is_syntax_error():
        assert issubclass(x.LayoutCompileError, SyntaxError)

    def check_parse_chord_simple():
        mod, usage = x.parse_chord("a")
        assert mod == 0 and usage == 0x04

    def check_parse_chord_shifted_letter():
        mod, usage = x.parse_chord("SHIFT+a")
        assert mod == (1 << 1) and usage == 0x04

    def check_parse_chord_space_token():
        mod, usage = x.parse_chord("SPACE")
        assert mod == 0 and usage == 0x2C

    def check_compile_minimal_header_and_entry():
        src = "name: t\na a\n"
        buf = x.compile_layout_source("minimal.layout", src)
        assert buf[0:4] == b"X52L"
        assert struct.unpack_from("!H", buf, 4)[0] == x.FORMAT_VERSION
        assert struct.unpack_from("!H", buf, 6)[0] == 0
        limit = struct.unpack_from("!I", buf, 8)[0]
        assert limit == 98
        assert len(buf) == x.HEADER_BYTES + 2 * limit
        off = x.HEADER_BYTES + 2 * ord('a')
        assert buf[off] == 0 and buf[off + 1] == 0x04
        stored = struct.unpack_from("!I", buf, 12)[0]
        assert stored == _x52_crc32_verify(buf)

    def check_compile_name_and_description_fields():
        src = "name: demo\ndescription: hello\nU+0020 SPACE\n"
        buf = x.compile_layout_source("meta.layout", src)
        name_end = buf.index(b"\0", 16)
        assert buf[16:name_end] == b"demo"
        desc_end = buf.index(b"\0", 48)
        assert buf[48:desc_end] == b"hello"
        space_off = x.HEADER_BYTES + 2 * 0x20
        assert buf[space_off + 1] == 0x2C

    def check_compile_shift_uppercase_mapping():
        src = "name: u\nA SHIFT+a\n"
        buf = x.compile_layout_source("up.layout", src)
        limit = struct.unpack_from("!I", buf, 8)[0]
        assert limit == ord("A") + 1
        off = x.HEADER_BYTES + 2 * ord("A")
        assert buf[off] == (1 << 1) and buf[off + 1] == 0x04

    def check_reject_missing_name():
        try:
            x.compile_layout_source("bad.layout", "a a\n")
        except x.LayoutCompileError as e:
            assert "missing name" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_no_mappings():
        try:
            x.compile_layout_source("bad.layout", "name: x\n")
        except x.LayoutCompileError as e:
            assert "no mappings" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_duplicate_codepoint():
        src = "name: x\na a\na b\n"
        try:
            x.compile_layout_source("dup.layout", src)
        except x.LayoutCompileError as e:
            assert "duplicate" in str(e) and "U+0061" in str(e)
            assert "line" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_invalid_scalar():
        src = "name: x\nU+D800 SPACE\n"
        try:
            x.compile_layout_source("surrogate.layout", src)
        except x.LayoutCompileError as e:
            assert "invalid Unicode scalar" in str(e)
            assert e.filename == "surrogate.layout"
            assert e.lineno == 2
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_scalar_above_unicode_max():
        src = "name: x\nU+110000 SPACE\n"
        try:
            x.compile_layout_source("high.layout", src)
        except x.LayoutCompileError as e:
            assert "invalid Unicode scalar" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_bad_chord_line():
        src = "name: x\na NOT_A_KEY\n"
        try:
            x.compile_layout_source("chord.layout", src)
        except x.LayoutCompileError as e:
            assert "unknown key token" in str(e)
            assert e.lineno == 2
        else:
            assert False, "expected LayoutCompileError"

    def check_reject_oversize_name():
        long_name = "n" * 32
        src = "name: %s\na a\n" % long_name
        try:
            x.compile_layout_source("long.layout", src)
        except x.LayoutCompileError as e:
            assert "exceeds" in str(e) and "UTF-8" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_default_allows_name_at_31_bytes():
        name = "n" * 31
        src = "name: %s\na a\n" % name
        buf = x.compile_layout_source("ok31.layout", src)
        name_end = buf.index(b"\0", 16)
        assert buf[16:name_end] == name.encode("utf-8")
        assert struct.unpack_from("!H", buf, 6)[0] == 0

    def check_default_allows_description_at_63_bytes():
        desc = "d" * 63
        src = "name: ok\ndescription: %s\na a\n" % desc
        buf = x.compile_layout_source("ok63.layout", src)
        desc_end = buf.index(b"\0", 48)
        assert buf[48:desc_end] == desc.encode("utf-8")
        assert struct.unpack_from("!H", buf, 6)[0] == 0

    def check_reject_invalid_layout_name():
        for bad in ("bad/name", "a b", "caf\u00e9", "dot."):
            src = "name: %s\na a\n" % bad
            try:
                x.compile_layout_source("name.layout", src)
            except x.LayoutCompileError as e:
                assert "ASCII letters" in str(e) or "hyphen" in str(e)
            else:
                assert False, "expected LayoutCompileError for %r" % bad

    def check_compile_name_hyphen_underscore():
        src = "name: a-b_9\na a\n"
        buf = x.compile_layout_source("hy.layout", src)
        name_end = buf.index(b"\0", 16)
        assert buf[16:name_end] == b"a-b_9"

    def check_reject_oversize_description():
        long_desc = "d" * 64
        src = "name: x\ndescription: %s\na a\n" % long_desc
        try:
            x.compile_layout_source("longdesc.layout", src)
        except x.LayoutCompileError as e:
            assert "description exceeds" in str(e)
        else:
            assert False, "expected LayoutCompileError"

    def check_truncate_metadata_sets_name_flag_and_field():
        long_name = "n" * 32
        src = "name: %s\na a\n" % long_name
        buf = x.compile_layout_source("trunc.layout", src, truncate_metadata=True)
        assert struct.unpack_from("!H", buf, 6)[0] == x.FLAG_NAME_TRUNCATED
        name_end = buf.index(b"\0", 16)
        assert buf[16:name_end] == (b"n" * 31)
        assert struct.unpack_from("!I", buf, 12)[0] == _x52_crc32_verify(buf)

    def check_truncate_metadata_sets_description_flag():
        long_desc = "d" * 64
        src = "name: ok\ndescription: %s\na a\n" % long_desc
        buf = x.compile_layout_source("truncdesc.layout", src, truncate_metadata=True)
        assert struct.unpack_from("!H", buf, 6)[0] == x.FLAG_DESCRIPTION_TRUNCATED
        desc_end = buf.index(b"\0", 48)
        assert buf[48:desc_end] == (b"d" * 63)

    def check_truncate_metadata_sets_both_flags_when_both_long():
        long_name = "n" * 32
        long_desc = "d" * 64
        src = "name: %s\ndescription: %s\na a\n" % (long_name, long_desc)
        buf = x.compile_layout_source("bothtrunc.layout", src, truncate_metadata=True)
        f = struct.unpack_from("!H", buf, 6)[0]
        assert f == (x.FLAG_NAME_TRUNCATED | x.FLAG_DESCRIPTION_TRUNCATED)
        name_end = buf.index(b"\0", 16)
        assert buf[16:name_end] == (b"n" * 31)
        desc_end = buf.index(b"\0", 48)
        assert buf[48:desc_end] == (b"d" * 63)
        assert struct.unpack_from("!I", buf, 12)[0] == _x52_crc32_verify(buf)

    def check_truncate_metadata_utf8_safe_no_split():
        # Names are ASCII-only; exercise UTF-8 trim on description (32 × "é" → 63-byte cap).
        long_desc = "\u00e9" * 32
        src = "name: ok\ndescription: %s\na a\n" % long_desc
        buf = x.compile_layout_source("utf8.layout", src, truncate_metadata=True)
        assert struct.unpack_from("!H", buf, 6)[0] == x.FLAG_DESCRIPTION_TRUNCATED
        desc_end = buf.index(b"\0", 48)
        assert buf[48:desc_end].decode("utf-8") == "\u00e9" * 31

    def check_data_layouts_us_layout_compiles():
        us_path = root / "data" / "layouts" / "us.layout"
        if not us_path.is_file():
            return
        text = us_path.read_text(encoding="utf-8")
        buf = x.compile_layout_source(str(us_path), text)
        assert buf[0:4] == b"X52L"
        limit = struct.unpack_from("!I", buf, 8)[0]
        assert len(buf) == x.HEADER_BYTES + 2 * limit
        assert struct.unpack_from("!I", buf, 12)[0] == _x52_crc32_verify(buf)

    def check_compile_layout_file_rejects_mismatched_output_basename():
        with tempfile.TemporaryDirectory() as td:
            src = pathlib.Path(td) / "in.layout"
            src.write_text("name: us\na a\n", encoding="utf-8")
            bad = pathlib.Path(td) / "other.x52l"
            try:
                x.compile_layout_file(str(src), str(bad))
            except x.LayoutCompileError as e:
                assert "us.x52l" in str(e) and "other.x52l" in str(e)
            else:
                assert False, "expected LayoutCompileError"

    def check_compile_layout_file_accepts_matching_basename():
        with tempfile.TemporaryDirectory() as td:
            src = pathlib.Path(td) / "in.layout"
            src.write_text("name: us\na a\n", encoding="utf-8")
            out = pathlib.Path(td) / "us.x52l"
            x.compile_layout_file(str(src), str(out))
            assert out.is_file()
            assert out.stat().st_size == x.HEADER_BYTES + 2 * 98

    def check_compile_layout_file_truncated_name_sets_output_basename():
        long_name = "n" * 32
        with tempfile.TemporaryDirectory() as td:
            src = pathlib.Path(td) / "in.layout"
            src.write_text("name: %s\na a\n" % long_name, encoding="utf-8")
            # Stored name is 31 × "n" → file must be nnn... (31) + .x52l
            out = pathlib.Path(td) / ("n" * 31 + ".x52l")
            x.compile_layout_file(str(src), str(out), truncate_metadata=True)
            assert out.is_file()

    def check_cli_truncate_metadata_sets_flags_in_output():
        long_name = "n" * 32
        long_desc = "d" * 64
        script = root / "tools" / "x52compile_layout.py"
        with tempfile.TemporaryDirectory() as td:
            td = pathlib.Path(td)
            src = td / "in.layout"
            src.write_text(
                "name: %s\ndescription: %s\na a\n" % (long_name, long_desc),
                encoding="utf-8",
            )
            out = td / ("n" * 31 + ".x52l")
            r = subprocess.run(
                [sys.executable, str(script), "--truncate-metadata", str(src), str(out)],
                cwd=str(root),
                capture_output=True,
                text=True,
                check=False,
            )
            assert r.returncode == 0, r.stderr
            buf = out.read_bytes()
            f = struct.unpack_from("!H", buf, 6)[0]
            assert f == (x.FLAG_NAME_TRUNCATED | x.FLAG_DESCRIPTION_TRUNCATED)
            assert struct.unpack_from("!I", buf, 12)[0] == _x52_crc32_verify(buf)

    tests = (
        ("LayoutCompileError subclasses SyntaxError", check_layout_compile_error_is_syntax_error),
        ("parse_chord(a)", check_parse_chord_simple),
        ("parse_chord(SHIFT+a)", check_parse_chord_shifted_letter),
        ("parse_chord(SPACE)", check_parse_chord_space_token),
        ("compile minimal layout: header, size, entry, CRC", check_compile_minimal_header_and_entry),
        ("compile name/description and U+ mapping", check_compile_name_and_description_fields),
        ("compile SHIFT+a for uppercase letter", check_compile_shift_uppercase_mapping),
        ("reject missing name:", check_reject_missing_name),
        ("reject no mappings", check_reject_no_mappings),
        ("reject duplicate codepoint", check_reject_duplicate_codepoint),
        ("reject surrogate scalar U+D800", check_reject_invalid_scalar),
        ("reject scalar U+110000", check_reject_scalar_above_unicode_max),
        ("reject unknown key token with lineno", check_reject_bad_chord_line),
        ("reject name longer than 31 UTF-8 bytes", check_reject_oversize_name),
        ("default: name length 31 UTF-8 bytes accepted", check_default_allows_name_at_31_bytes),
        ("default: description length 63 UTF-8 bytes accepted", check_default_allows_description_at_63_bytes),
        ("reject layout name outside ASCII alnum hyphen underscore", check_reject_invalid_layout_name),
        ("compile name with hyphen and underscore", check_compile_name_hyphen_underscore),
        ("reject description longer than 63 UTF-8 bytes", check_reject_oversize_description),
        ("--truncate-metadata: name flag and 31-byte field", check_truncate_metadata_sets_name_flag_and_field),
        ("truncate_metadata: description flag and 63-byte field", check_truncate_metadata_sets_description_flag),
        ("truncate_metadata: both long name and description set both flags", check_truncate_metadata_sets_both_flags_when_both_long),
        ("truncate_metadata: UTF-8 safe trim (no split codepoint)", check_truncate_metadata_utf8_safe_no_split),
        ("CLI --truncate-metadata sets composite flags and valid CRC", check_cli_truncate_metadata_sets_flags_in_output),
        ("data/layouts/us.layout compiles with valid CRC", check_data_layouts_us_layout_compiles),
        ("compile_layout_file rejects wrong output basename", check_compile_layout_file_rejects_mismatched_output_basename),
        ("compile_layout_file writes when basename matches name:", check_compile_layout_file_accepts_matching_basename),
        ("compile_layout_file: truncated stored name defines output basename", check_compile_layout_file_truncated_name_sets_output_basename),
    )

    return _emit_tap(tests)


if __name__ == "__main__":
    raise SystemExit(main())
