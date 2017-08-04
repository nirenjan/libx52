#!/bin/bash
# Test setting MFD brightness to 0x005c

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x005c

$X52CLI bri MFD 0x005c

verify_output

