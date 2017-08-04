#!/bin/bash
# Test setting blink indicator ON

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_BLINK_INDICATOR_INDEX $X52_INDICATOR_STATE_ON

$X52CLI BLINK ON

verify_output

