#!/bin/bash
# Test setting MFD brightness to 0x000e

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x000e

$X52CLI bri MFD 0x000e

verify_output
