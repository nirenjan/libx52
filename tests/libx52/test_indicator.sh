#!/bin/bash

source $(dirname $0)/../common_infra.sh

TEST_SUITE_ID="libx52 indicator tests (blink & shift)"

indicator_test()
{
    local indicator=$(echo $1 | tr a-z A-Z)
    local state=$(echo $2 | tr a-z A-Z)
    local index="\$X52_${indicator}_INDICATOR_INDEX"
    local value="\$X52_INDICATOR_STATE_${state}"
    TEST_ID="Test setting $indicator $state"

    expect_pattern `eval echo $index $value`

    $X52CLI $indicator $state

    verify_output
}

for indicator in blink shift
do
    for state in on off
    do
        indicator_test $indicator $state
    done
done

verify_test_suite

