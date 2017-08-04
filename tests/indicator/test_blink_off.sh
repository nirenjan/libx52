#!/bin/bash
# Test setting blink indicator OFF

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_BLINK_INDICATOR_INDEX $X52_INDICATOR_STATE_OFF

$X52CLI BLINK OFF

verify_output

