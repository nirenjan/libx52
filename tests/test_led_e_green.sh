#!/bin/bash
# Test setting the E button to green

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_E_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_E_GREEN_ON

$X52CLI led E green

verify_output

