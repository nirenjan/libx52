#!/bin/bash
# Test setting the T2 button to green

source $(dirname $0)/../common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_GREEN_ON

$X52CLI led T2 green

verify_output

