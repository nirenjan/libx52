#!/bin/bash
# Test setting the THROTTLE button off

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_THROTTLE_OFF

$X52CLI led THROTTLE off

verify_output

