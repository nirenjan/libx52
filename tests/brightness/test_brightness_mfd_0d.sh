#!/bin/bash
# Test setting MFD brightness to 0x000d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x000d

$X52CLI bri MFD 0x000d

verify_output

