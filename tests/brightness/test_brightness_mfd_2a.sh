#!/bin/bash
# Test setting MFD brightness to 0x002a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x002a

$X52CLI bri MFD 0x002a

verify_output

