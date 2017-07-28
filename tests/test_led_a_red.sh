#!/bin/bash
# Test setting the A button to red

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_A_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_A_GREEN_OFF

$X52CLI led A red

verify_output

