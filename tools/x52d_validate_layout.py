#!/usr/bin/env python3
"""Validate a compiled layout file, and optionally dump the contents"""

import argparse
import struct
import zlib

class X52LayoutError(Exception):
    """Exception raised when there's a flaw in the layout"""

class X52Layout:
    """X52Layout is a class that parses a compiled layout file, validates it
       and dumps the mappings"""

    def __init__(self, layout_file):
        """Load the layout from the layout file"""
        with open(layout_file, 'rb') as lfd:
            self.raw_layout = lfd.read()

        if len(self.raw_layout) < 128:
            raise X52LayoutError("layout file too short (%d bytes)" % len(self.raw_layout))

        self.magic = self._load_magic()
        self.version = self._load_version()
        self.flags = self._load_flags()
        self.count = self._load_count()

        self._validate_checksum()
        self.name = self._load_layout_name()
        self.description = self._load_layout_description()

    def _load_magic(self):
        """Load and validate the magic bytes"""
        magic = struct.unpack_from('4s', self.raw_layout[0:4])
        if magic[0] != b'X52L':
            raise X52LayoutError("invalid magic identifier: (%s)" % magic)

        return magic

    def _load_version(self):
        """Load and validate the version bytes"""
        version = struct.unpack_from(">H", self.raw_layout[4:6])
        if version[0] != 1:
            raise X52LayoutError("invalid version %d" % version)

        return version[0]

    def _load_flags(self):
        """Load and validate the flags"""
        flags = struct.unpack_from('>H', self.raw_layout[6:8])[0]
        should_be_zero = flags >> 2
        if should_be_zero:
            raise X52LayoutError("reserved flags set; upper bits should be 0; got 0x%04x" % flags)

        return flags

    def _load_count(self):
        """Load and validate the count"""
        count = struct.unpack_from('>I', self.raw_layout[8:12])[0]
        if count == 0 or count > 0x10FFFF:
            raise X52LayoutError("invalid count 0x%x" % count)

        expected_length = count * 2 + 128
        if len(self.raw_layout) < expected_length:
            raise X52LayoutError("Insufficient bytes: expected %d; got %d",
                                 expected_length, len(self.raw_layout))

        if len(self.raw_layout) > expected_length:
            raise X52LayoutError("Input too long: expected %d; got %d",
                                 expected_length, len(self.raw_layout))

        return count

    def _validate_checksum(self):
        """Load and validate the checksum"""
        checksum = struct.unpack_from('>I', self.raw_layout[12:16])[0]
        computed = zlib.crc32(self.raw_layout[:12])
        computed = zlib.crc32(b'\0\0\0\0', computed)
        computed = zlib.crc32(self.raw_layout[16:], computed) & 0xFFFFFFFF
        if checksum != computed:
            raise X52LayoutError("corrupted checksum; expected %04x; got %04x" % (checksum, computed))

    @staticmethod
    def _read_nul_terminated_string(array):
        """Read a nul terminated string from an array"""
        format_str = f"{len(array)}s"
        raw_string = struct.unpack_from(format_str, array)[0]
        return raw_string.rstrip(b'\x00').decode('utf-8')

    def _load_layout_name(self):
        """Load the name from the layout"""
        name = self._read_nul_terminated_string(self.raw_layout[16:48])
        if not name:
            raise X52LayoutError("Missing layout name")

        return name

    def _load_layout_description(self):
        """Load the description from the layout"""
        return self._read_nul_terminated_string(self.raw_layout[48:112])

    def dump(self):
        """Dump the parsed layout file"""
        print("name:", self.name)
        print("description:", self.description)
        print("magic:", self.magic)
        print("version:", self.version)
        print("flags: %#x" % self.flags)
        print("count:", self.count)

def main():
    """Main function"""
    parser = argparse.ArgumentParser(description="Parse and validate x52layout file")
    parser.add_argument('-d', '--dump', action='store_true',
                        help='Dump decoded layout')
    parser.add_argument('layout_file', metavar='layout-file',
                        help='Path to compiled layout file')
    args = parser.parse_args()

    layout = X52Layout(args.layout_file)
    if args.dump:
        layout.dump()

if __name__ == '__main__':
    main()
