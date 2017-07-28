#!/bin/bash
# Test setting the B button to green

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_B_RED_OFF
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_B_GREEN_ON

$X52CLI led B green

verify_output

