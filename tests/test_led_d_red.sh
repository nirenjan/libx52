#!/bin/bash
# Test setting the D button to red

source $(dirname $0)/common_infra.sh

expect_pattern \
    $X52_LED_COMMAND_INDEX $X52_LED_D_RED_ON \
    $X52_LED_COMMAND_INDEX $X52_LED_D_GREEN_OFF

$X52CLI led D red

verify_output

