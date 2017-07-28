#!/bin/bash
# Test setting the T2 button to amber

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_GREEN_ON

$X52CLI led T2 amber

verify_output

