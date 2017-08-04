#!/bin/bash
# Test setting MFD brightness to 0x0034

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x0034

$X52CLI bri MFD 0x0034

verify_output

