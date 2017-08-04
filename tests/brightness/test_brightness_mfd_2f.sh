#!/bin/bash
# Test setting MFD brightness to 0x002f

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x002f

$X52CLI bri MFD 0x002f

verify_output

