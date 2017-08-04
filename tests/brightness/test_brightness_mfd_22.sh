#!/bin/bash
# Test setting MFD brightness to 0x0022

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x0022

$X52CLI bri MFD 0x0022

verify_output

