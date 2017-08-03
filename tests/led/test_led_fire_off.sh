#!/bin/bash
# Test setting the FIRE button off

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_FIRE_OFF

$X52CLI led FIRE off

verify_output

