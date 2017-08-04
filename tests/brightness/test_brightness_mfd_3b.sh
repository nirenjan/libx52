#!/bin/bash
# Test setting MFD brightness to 0x003b

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x003b

$X52CLI bri MFD 0x003b

verify_output

