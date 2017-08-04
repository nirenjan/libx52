#!/bin/bash
# Test setting MFD brightness to 0x0058

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x0058

$X52CLI bri MFD 0x0058

verify_output

