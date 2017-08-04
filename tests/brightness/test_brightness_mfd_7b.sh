#!/bin/bash
# Test setting MFD brightness to 0x007b

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x007b

$X52CLI bri MFD 0x007b

verify_output

