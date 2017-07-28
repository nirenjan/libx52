#!/bin/bash
# Test setting the E button to amber

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_E_RED_ON
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_E_GREEN_ON

$X52CLI led E amber

verify_output

