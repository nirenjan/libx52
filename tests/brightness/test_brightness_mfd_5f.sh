#!/bin/bash
# Test setting MFD brightness to 0x005f

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x005f

$X52CLI bri MFD 0x005f

verify_output

