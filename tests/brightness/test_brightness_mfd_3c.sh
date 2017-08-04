#!/bin/bash
# Test setting MFD brightness to 0x003c

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x003c

$X52CLI bri MFD 0x003c

verify_output

