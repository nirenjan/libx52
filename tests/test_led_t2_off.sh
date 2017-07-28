#!/bin/bash
# Test setting the T2 button to off

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_T2_RED_OFF
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_T2_GREEN_OFF

$X52CLI led T2 off

verify_output

