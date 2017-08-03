#!/bin/bash
# Test setting the D button to amber

source $(dirname $0)/../common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_D_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_D_GREEN_ON

$X52CLI led D amber

verify_output

