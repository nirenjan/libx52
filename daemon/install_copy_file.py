#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0
"""Meson install_script helper: copy one file to DESTDIR + absolute install path."""
import os
import shutil
import sys


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit('usage: install_copy_file.py SRC DEST_ABS')
    src = sys.argv[1]
    dest_abs = sys.argv[2]
    destdir = os.environ.get('DESTDIR', '')
    if destdir:
        dest = os.path.normpath(os.path.join(destdir, dest_abs.lstrip(os.sep)))
    else:
        dest = dest_abs
    os.makedirs(os.path.dirname(dest), exist_ok=True)
    shutil.copyfile(src, dest)


if __name__ == '__main__':
    main()
