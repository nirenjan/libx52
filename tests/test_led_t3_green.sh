#!/bin/bash
# Test setting the T3 button to green

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_T3_RED_OFF
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_T3_GREEN_ON

$X52CLI led T3 green

verify_output

