#!/bin/bash
# Test setting MFD brightness to 0x003a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x003a

$X52CLI bri MFD 0x003a

verify_output

