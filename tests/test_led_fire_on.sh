#!/bin/bash
# Test setting the FIRE button on

source $(dirname $0)/common_infra.sh

expect_pattern $X52_LED_COMMAND_INDEX $X52_LED_FIRE_ON

$X52CLI led FIRE on

verify_output

