#!/bin/bash
# Test setting the E button to off

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_E_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_E_GREEN_OFF

$X52CLI led E off

verify_output

