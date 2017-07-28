#!/bin/bash
# Test setting the POV button to amber

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_POV_RED_ON
expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_POV_GREEN_ON

$X52CLI led POV amber

verify_output

