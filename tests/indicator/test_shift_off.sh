#!/bin/bash
# Test setting shift indicator OFF

source $(dirname $0)/../common_infra.sh

expect_pattern $X52_SHIFT_INDICATOR_INDEX $X52_INDICATOR_STATE_OFF

$X52CLI SHIFT OFF

verify_output

