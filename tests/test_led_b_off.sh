#!/bin/bash
# Test setting the B button to off

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_B_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_B_GREEN_OFF

$X52CLI led B off

verify_output

