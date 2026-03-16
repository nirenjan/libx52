#!/bin/sh

TEST_RUNNER="./libx52util-bmp-test"
TEST_BIN="./libx52util/x52_char_map.bin"

if [ -e "${TEST_RUNNER}" ] && [ -e "${TEST_BIN}" ];
then
    "${TEST_RUNNER}" "${TEST_BIN}"
else
    echo "TAP version 13"
    echo "1..0 # missing files"
fi
