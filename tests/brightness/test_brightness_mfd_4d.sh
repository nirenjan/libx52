#!/bin/bash
# Test setting MFD brightness to 0x004d

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_MFD_BRIGHTNESS_INDEX 0x004d

$X52CLI bri MFD 0x004d

verify_output

