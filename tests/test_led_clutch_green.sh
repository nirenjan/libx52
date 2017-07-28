#!/bin/bash
# Test setting the CLUTCH button to green

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_CLUTCH_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_CLUTCH_GREEN_ON

$X52CLI led CLUTCH green

verify_output

