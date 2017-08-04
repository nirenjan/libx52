#!/bin/bash
# Test setting MFD brightness to 0x004b

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x004b

$X52CLI bri MFD 0x004b

verify_output

