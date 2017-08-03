#!/bin/bash
# Test setting the T2 button to red

source $(dirname $0)/../common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_T2_GREEN_OFF

$X52CLI led T2 red

verify_output

