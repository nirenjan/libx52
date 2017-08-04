#!/bin/bash
# Test setting MFD brightness to 0x001a

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x001a

$X52CLI bri MFD 0x001a

verify_output

