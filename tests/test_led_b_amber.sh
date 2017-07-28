#!/bin/bash
# Test setting the B button to amber

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_B_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_B_GREEN_ON

$X52CLI led B amber

verify_output

