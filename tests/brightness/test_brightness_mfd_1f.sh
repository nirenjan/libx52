#!/bin/bash
# Test setting MFD brightness to 0x001f

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x001f

$X52CLI bri MFD 0x001f

verify_output

