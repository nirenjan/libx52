#!/bin/bash
# Test setting the D button to green

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_D_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_D_GREEN_ON

$X52CLI led D green

verify_output

