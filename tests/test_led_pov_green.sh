#!/bin/bash
# Test setting the POV button to green

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_POV_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_POV_GREEN_ON

$X52CLI led POV green

verify_output

