#!/bin/bash
# Test setting MFD brightness to 0x006d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x006d

$X52CLI bri MFD 0x006d

verify_output

