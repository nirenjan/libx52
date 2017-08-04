#!/bin/bash
# Test setting shift indicator ON

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_SHIFT_INDICATOR_INDEX $X52_INDICATOR_STATE_ON

$X52CLI SHIFT ON

verify_output

