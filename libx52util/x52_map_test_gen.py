#!/usr/bin/env python3
"""Generate a test script for the convert function"""

import argparse
import re

def parse_file(map_file):
    """Read the map file, strip out comments, and return a dictionary that
       maps the UTF-8 encoded string to the X52 MFD character"""

    # If we are running this, then we know that the input map is likely
    # in a sane format already.
    char_dict = {}

    with open(map_file, 'r', encoding='utf-8') as map_fd:
        for line in map_fd:
            line = re.sub(r'#.*$', '', line).strip()

            if not line:
                # Comment line, skip
                continue

            key, out = line.split()
            in_char = int(key, 0)

            if len(out) == 1:
                out_byte = ord(out)
            else:
                out_byte = int(out, 0)

            char_dict[in_char] = out_byte

    return char_dict

def generate_positive_test_cases(char_dict):
    """Generate a set of positive test cases"""
    # For every string in the dictionary, generate a test case that tests
    # the input against the output
    TEST_CASE_FMT = """
static void test_map_{in_char}(void **state) {{
    (void)state;
    const uint8_t input_array[] = {{ {in_bytes}, 0 }};
    const uint8_t expected_output[2] = {{ {out_byte}, 0 }};
    size_t out_len = 20;
    uint8_t output[20] = {{ 0 }};
    int rc;

    rc = libx52util_convert_utf8_string(input_array, output, &out_len);
    assert_int_equal(rc, 0);
    assert_int_equal(out_len, 1);
    assert_memory_equal(output, expected_output, 2);
}}
"""

    output = ""
    for in_char, out_byte in char_dict.items():
        in_bytes = ", ".join(hex(c) for c in chr(in_char).encode('utf-8'))
        in_tc = hex(in_char)

        output += TEST_CASE_FMT.format(in_char=in_tc, in_bytes=in_bytes, out_byte=out_byte)

    output += """
const struct CMUnitTest tests[] = {
"""

    for in_char in sorted(char_dict.keys()):
        output += f"    cmocka_unit_test(test_map_{hex(in_char)}),\n"

    output += '};\n'

    return output

TEST_HEADER = """
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>

#include "libx52util.h"
"""

TEST_FOOTER = """
int main(void) {
    cmocka_set_message_output(CM_OUTPUT_TAP);
    cmocka_run_group_tests(tests, NULL, NULL);
    return 0;
}
"""

def main():
    """Generate X52 map test suite"""
    parser = argparse.ArgumentParser(description='Generate map test cases')
    parser.add_argument('INPUT_FILE', help="Input character map file")
    parser.add_argument('OUTPUT_FILE', help="Generated test script")
    args = parser.parse_args()

    char_dict = parse_file(args.INPUT_FILE)
    test_cases = generate_positive_test_cases(char_dict)

    with open(args.OUTPUT_FILE, 'w', encoding='utf-8') as out_fd:
        print(TEST_HEADER, file=out_fd)
        print(test_cases, file=out_fd)
        print(TEST_FOOTER, file=out_fd)

if __name__ == '__main__':
    main()
