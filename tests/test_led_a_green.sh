#!/bin/bash
# Test setting the A button to green

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_A_RED_OFF
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_A_GREEN_ON

$X52CLI led A green

verify_output

