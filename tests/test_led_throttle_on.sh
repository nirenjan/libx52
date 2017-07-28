#!/bin/bash
# Test setting the THROTTLE button on

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_THROTTLE_ON

$X52CLI led THROTTLE on

verify_output

