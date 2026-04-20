#!/usr/bin/env python3
# Copyright (C) 2026 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
"""Regression checks for layout HID allowlist and token maps (see vkm.h / layout_usage_allowlist.c)."""

import pathlib
import re
import sys


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


def main():
    root = pathlib.Path(__file__).resolve().parent.parent
    sys.path.insert(0, str(root / "tools"))

    import x52compile_layout as x

    def check_allowed_set():
        expected = frozenset(range(0x04, 0x3A)) | frozenset((0x64,))
        assert x.ALLOWED_KEY_USAGES == expected, "%r != %r" % (x.ALLOWED_KEY_USAGES, expected)

    def check_allowlist_c_range():
        src = (root / "daemon" / "layout_usage_allowlist.c").read_text(encoding="utf-8")
        assert "usage >= 0x04 && usage <= 0x39" in src, (
            "daemon/layout_usage_allowlist.c: expected main-block range check"
        )

    def check_allowlist_c_intl():
        src = (root / "daemon" / "layout_usage_allowlist.c").read_text(encoding="utf-8")
        assert "usage == 0x64" in src, (
            "daemon/layout_usage_allowlist.c: expected INTL_BACKSLASH (0x64) check"
        )

    def check_vkm_modifier_symbols():
        vkm = (root / "include" / "vkm" / "vkm.h").read_text(encoding="utf-8")
        for name in (
            "VKM_KEY_MOD_LCTRL",
            "VKM_KEY_MOD_LSHIFT",
            "VKM_KEY_MOD_LALT",
            "VKM_KEY_MOD_LGUI",
            "VKM_KEY_MOD_RCTRL",
            "VKM_KEY_MOD_RSHIFT",
            "VKM_KEY_MOD_RALT",
            "VKM_KEY_MOD_RGUI",
        ):
            assert re.search(r"%s\s*=\s*\([^)]+\)" % re.escape(name), vkm), (
                "could not find %s in vkm.h" % name
            )

    def check_modifier_bits():
        assert x.MODIFIER_TOKEN_TO_BIT["LCTRL"] == 1 << 0 and x.MODIFIER_TOKEN_TO_BIT["RALT"] == 1 << 6, (
            "modifier map mismatch vs expected HID bit layout"
        )

    def check_tokens_in_allowlist():
        for _tok, usage in x.KEY_TOKEN_TO_USAGE.items():
            assert usage in x.ALLOWED_KEY_USAGES, (
                "token maps outside allowlist: %r -> %#x" % (_tok, usage)
            )

    def check_usage_from_key_token_a():
        assert x.usage_from_key_token("a") == 0x04

    def check_usage_from_key_token_space():
        assert x.usage_from_key_token("SPACE") == 0x2C

    def check_modifier_mask_ctrl_shift():
        assert x.modifier_mask_from_tokens("CTRL+SHIFT") == (1 << 0) | (1 << 1)

    tests = (
        ("ALLOWED_KEY_USAGES matches main block + INTL_BACKSLASH", check_allowed_set),
        ("layout_usage_allowlist.c main-block range", check_allowlist_c_range),
        ("layout_usage_allowlist.c INTL_BACKSLASH (0x64)", check_allowlist_c_intl),
        ("vkm.h modifier symbol names", check_vkm_modifier_symbols),
        ("MODIFIER_TOKEN_TO_BIT LCTRL/RALT bits", check_modifier_bits),
        ("KEY_TOKEN_TO_USAGE subset of allowlist", check_tokens_in_allowlist),
        ("usage_from_key_token(a)", check_usage_from_key_token_a),
        ("usage_from_key_token(SPACE)", check_usage_from_key_token_space),
        ("modifier_mask_from_tokens(CTRL+SHIFT)", check_modifier_mask_ctrl_shift),
    )

    return _emit_tap(tests)


if __name__ == "__main__":
    raise SystemExit(main())
