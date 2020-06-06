#!/bin/bash
# Common infrastructure for the test cases
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

# Set up some command sequences
X52_LED_COMMAND_INDEX='00b8'
X52_LED_FIRE_ON='0101'
X52_LED_FIRE_OFF='0100'
X52_LED_A_RED_ON='0201'
X52_LED_A_RED_OFF='0200'
X52_LED_A_GREEN_ON='0301'
X52_LED_A_GREEN_OFF='0300'
X52_LED_B_RED_ON='0401'
X52_LED_B_RED_OFF='0400'
X52_LED_B_GREEN_ON='0501'
X52_LED_B_GREEN_OFF='0500'
X52_LED_D_RED_ON='0601'
X52_LED_D_RED_OFF='0600'
X52_LED_D_GREEN_ON='0701'
X52_LED_D_GREEN_OFF='0700'
X52_LED_E_RED_ON='0801'
X52_LED_E_RED_OFF='0800'
X52_LED_E_GREEN_ON='0901'
X52_LED_E_GREEN_OFF='0900'
X52_LED_T1_RED_ON='0a01'
X52_LED_T1_RED_OFF='0a00'
X52_LED_T1_GREEN_ON='0b01'
X52_LED_T1_GREEN_OFF='0b00'
X52_LED_T2_RED_ON='0c01'
X52_LED_T2_RED_OFF='0c00'
X52_LED_T2_GREEN_ON='0d01'
X52_LED_T2_GREEN_OFF='0d00'
X52_LED_T3_RED_ON='0e01'
X52_LED_T3_RED_OFF='0e00'
X52_LED_T3_GREEN_ON='0f01'
X52_LED_T3_GREEN_OFF='0f00'
X52_LED_POV_RED_ON='1001'
X52_LED_POV_RED_OFF='1000'
X52_LED_POV_GREEN_ON='1101'
X52_LED_POV_GREEN_OFF='1100'
X52_LED_CLUTCH_RED_ON='1201'
X52_LED_CLUTCH_RED_OFF='1200'
X52_LED_CLUTCH_GREEN_ON='1301'
X52_LED_CLUTCH_GREEN_OFF='1300'
X52_LED_THROTTLE_ON='1401'
X52_LED_THROTTLE_OFF='1400'

X52_MFD_BRIGHTNESS_INDEX='00b1'
X52_LED_BRIGHTNESS_INDEX='00b2'

X52_BLINK_INDICATOR_INDEX='00b4'
X52_SHIFT_INDICATOR_INDEX='00fd'
X52_INDICATOR_STATE_ON='0051'
X52_INDICATOR_STATE_OFF='0050'

X52_CLOCK_1_INDEX='00c0'
X52_CLOCK_2_INDEX='00c1'
X52_CLOCK_3_INDEX='00c2'
X52_CLOCK_DATE_INDEX='00c4'
X52_CLOCK_YEAR_INDEX='00c8'

X52_MFD_LINE_0_SET_INDEX='00d1'
X52_MFD_LINE_1_SET_INDEX='00d2'
X52_MFD_LINE_2_SET_INDEX='00d4'
X52_MFD_LINE_0_CLR_INDEX='00d9'
X52_MFD_LINE_1_CLR_INDEX='00da'
X52_MFD_LINE_2_CLR_INDEX='00dc'

find_programs()
{
    # Tests and distcheck do not work on OSX, skip the tests
    if [[ `uname -s` == [Dd]arwin* ]]
    then
        echo "1..0 # skip Tests not supported on OSX"
        exit 0
    fi

    # Find the X52cli script
    X52CLI=$(find .. -path '*/cli/x52cli' -executable)

    if [[ -z "$X52CLI" ]]
    then
        exit 1
    fi

    # Find the x52test_log_actions program
    X52LOGACT=$(find .. -path '*/libusbx52/x52test_log_actions' -executable)
    if [[ -z "$X52LOGACT" ]]
    then
        exit 1
    fi

    # Find the x52test_create_device_list program
    X52DEVLIST=$(find .. -path '*/libusbx52/x52test_create_device_list' -executable)
    if [[ -z "$X52DEVLIST" ]]
    then
        exit 1
    fi
}

# This is for additional programs that are not needed by every test case
require_programs()
{
    local skip=false
    for prog in "$@"
    do
        if ! command -v "$prog"
        then
            echo "1..0 # skip Required program '$prog' not found"
            skip=true
        fi
    done

    if $skip
    then
        exit 0
    fi
}

setup_preload()
{
    # Find the libusb stub library
    LIBUSB=$(find .. -name 'libusbx52.so.*.*.*' -type f)

    if [[ -z "$LIBUSB" ]]
    then
        exit 1
    fi

    export LD_PRELOAD=$(realpath $LIBUSB)
}

setup_test()
{
    export LIBUSBX52_DEVICE_LIST=$(mktemp)
    EXPECTED_OUTPUT=$(mktemp)
    OBSERVED_OUTPUT=$(mktemp)
    trap "rm -f $EXPECTED_OUTPUT $OBSERVED_OUTPUT $LIBUSBX52_DEVICE_LIST" EXIT

    $X52DEVLIST 06a3 0762

    TEST_COUNT=0
    TEST_PASS=0
    TEST_FAIL=0
}

expect_pattern()
{
    TEST_NUM=$((TEST_NUM + 1))
    # Save pattern to expected output file
    export LIBUSBX52_OUTPUT_DATA=$EXPECTED_OUTPUT
    $X52LOGACT $@

    # Save actual API calls to observed output file
    export LIBUSBX52_OUTPUT_DATA=$OBSERVED_OUTPUT
}

verify_output()
{
    TEST_COUNT=$(($TEST_COUNT + 1))
    if diff -q $EXPECTED_OUTPUT $OBSERVED_OUTPUT
    then
        echo "ok $TEST_COUNT $TEST_ID"
        TEST_PASS=$(($TEST_PASS + 1))
    else
        echo "not ok $TEST_COUNT $TEST_ID"
        echo '# Expected:'
        echo '# ========='
        sed 's/^/#\t/' $EXPECTED_OUTPUT
        echo '#'
        echo '# Observed:'
        echo '# ========='
        sed 's/^/#\t/' $OBSERVED_OUTPUT
        TEST_FAIL=$(($TEST_FAIL + 1))
    fi
}

verify_test_suite()
{
    local sep='--------'
    sep="$sep$sep"
    sep="$sep$sep"
    sep="$sep$sep"

    echo '#' $sep
    echo '#' $TEST_SUITE_ID
    echo '#' $sep
    echo -e "# Total Tests:\t$TEST_COUNT"
    echo -e "# Tests Passed:\t$TEST_PASS"
    echo -e "# Tests Failed:\t$TEST_FAIL"
    echo '#' $sep

    echo "1..$TEST_COUNT"
}

set -e
find_programs
setup_test
setup_preload

