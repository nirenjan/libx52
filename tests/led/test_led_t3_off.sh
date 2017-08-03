#!/bin/bash
# Test setting the T3 button to off

source $(dirname $0)/../common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_T3_RED_OFF \
    $X52_LED_COMMAND_INDEX $X52_LED_T3_GREEN_OFF

$X52CLI led T3 off

verify_output

