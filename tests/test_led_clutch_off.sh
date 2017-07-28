#!/bin/bash
# Test setting the CLUTCH button to off

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_CLUTCH_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_CLUTCH_GREEN_OFF

$X52CLI led CLUTCH off

verify_output

