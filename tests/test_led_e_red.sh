#!/bin/bash
# Test setting the E button to red

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_E_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_E_GREEN_OFF

$X52CLI led E red

verify_output

