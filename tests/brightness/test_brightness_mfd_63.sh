#!/bin/bash
# Test setting MFD brightness to 0x0063

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x0063

$X52CLI bri MFD 0x0063

verify_output

