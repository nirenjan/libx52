#!/bin/bash
# Test setting the T1 button to amber

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_T1_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_T1_GREEN_ON

$X52CLI led T1 amber

verify_output

