#!/bin/bash
# Test setting LED brightness to 0x0033

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_LED_BRIGHTNESS_INDEX 0x0033

$X52CLI bri LED 0x0033

verify_output

