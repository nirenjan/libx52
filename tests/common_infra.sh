#!/bin/bash
# Common infrastructure for the test cases

# Set up exit status codes
EXIT_SUCCESS=0
EXIT_SKIP=77
EXIT_HARD_ERROR=99
EXIT_FAILURE=1

# Set up some command sequences
X52_LED_COMMAND_INDEX=00b8
X52_LED_FIRE_ON=0101
X52_LED_FIRE_OFF=0100
X52_LED_A_RED_ON=0201
X52_LED_A_RED_OFF=0200
X52_LED_A_GREEN_ON=0301
X52_LED_A_GREEN_OFF=0300
X52_LED_B_RED_ON=0401
X52_LED_B_RED_OFF=0400
X52_LED_B_GREEN_ON=0501
X52_LED_B_GREEN_OFF=0500
X52_LED_D_RED_ON=0601
X52_LED_D_RED_OFF=0600
X52_LED_D_GREEN_ON=0701
X52_LED_D_GREEN_OFF=0700
X52_LED_E_RED_ON=0801
X52_LED_E_RED_OFF=0800
X52_LED_E_GREEN_ON=0901
X52_LED_E_GREEN_OFF=0900
X52_LED_T1_RED_ON=0a01
X52_LED_T1_RED_OFF=0a00
X52_LED_T1_GREEN_ON=0b01
X52_LED_T1_GREEN_OFF=0b00
X52_LED_T2_RED_ON=0c01
X52_LED_T2_RED_OFF=0c00
X52_LED_T2_GREEN_ON=0d01
X52_LED_T2_GREEN_OFF=0d00
X52_LED_T3_RED_ON=0e01
X52_LED_T3_RED_OFF=0e00
X52_LED_T3_GREEN_ON=0f01
X52_LED_T3_GREEN_OFF=0f00
X52_LED_POV_RED_ON=1001
X52_LED_POV_RED_OFF=1000
X52_LED_POV_GREEN_ON=1101
X52_LED_POV_GREEN_OFF=1100
X52_LED_CLUTCH_RED_ON=1201
X52_LED_CLUTCH_RED_OFF=1200
X52_LED_CLUTCH_GREEN_ON=1301
X52_LED_CLUTCH_GREEN_OFF=1300
X52_LED_THROTTLE_ON=1401
X52_LED_THROTTLE_OFF=1400

find_programs()
{
    # Tests and distcheck do not work on OSX, skip the tests
    if [[ `uname -s` == [Dd]arwin* ]]
    then
        exit $EXIT_SKIP
    fi

    # Find the X52cli script
    X52CLI=$(find .. -path '*/cli/x52cli' -executable)

    if [[ -z "$X52CLI" ]]
    then
        exit $EXIT_HARD_ERROR
    fi

    # Find the x52test_log_actions program
    X52LOGACT=$(find .. -path '*/libusbx52/x52test_log_actions' -executable)
    if [[ -z "$X52LOGACT" ]]
    then
        exit $EXIT_HARD_ERROR
    fi

    # Find the x52test_create_device_list program
    X52DEVLIST=$(find .. -path '*/libusbx52/x52test_create_device_list' -executable)
    if [[ -z "$X52DEVLIST" ]]
    then
        exit $EXIT_HARD_ERROR
    fi
}

setup_preload()
{
    # Find the libusb stub library
    LIBUSB=$(find .. -name 'libusbx52.so.*.*.*' -type f)

    if [[ -z "$LIBUSB" ]]
    then
        exit $EXIT_HARD_ERROR
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
}

expect_pattern()
{
    # Save pattern to expected output file
    export LIBUSBX52_OUTPUT_DATA=$EXPECTED_OUTPUT
    $X52LOGACT $@

    # Save actual API calls to observed output file
    export LIBUSBX52_OUTPUT_DATA=$OBSERVED_OUTPUT
}

verify_output()
{
    if diff -q $EXPECTED_OUTPUT $OBSERVED_OUTPUT
    then
        exit $EXIT_SUCCESS
    else
        echo 'Expected:'
        echo '========='
        sed 's/^/\t/' $EXPECTED_OUTPUT
        echo
        echo 'Observed:'
        echo '========='
        sed 's/^/\t/' $OBSERVED_OUTPUT
        exit $EXIT_FAILURE
    fi
}

set -e
find_programs
setup_test
setup_preload

