#!/bin/bash
# LED tests
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

source $(dirname $0)/../common_infra.sh

TEST_SUITE_ID="libx52 LED state tests"

mono_led_test()
{
    local led_ident=$(echo $1 | tr a-z A-Z)
    local led_state=$2
    local state=$(echo "\$X52_LED_${led_ident}_${led_state}" | tr a-z A-Z)
    local TEST_ID="Test setting the $led_ident button $led_state"

    expect_pattern $X52_LED_COMMAND_INDEX `eval echo $state`

    $X52CLI led $led_ident $led_state

    verify_output
}

color_led_test()
{
    local led_ident=$(echo $1 | tr a-z A-Z)
    local led_color=$(echo $2 | tr A-Z a-z)
    local led_r_state=off
    local led_g_state=off
    local TEST_ID="Test setting the $led_ident button to $led_color"

    case $led_color in
    'red')
        led_r_state=on
        ;;
    'green')
        led_g_state=on
        ;;
    'amber')
        led_r_state=on
        led_g_state=on
        ;;
    'off')
        ;;
    *)
        echo "ERROR: Unknown ident" >&2
        exit $EXIT_HARD_ERROR
        ;;
    esac

    led_r_state=$(echo "\$X52_LED_${led_ident}_red_${led_r_state}" | tr a-z A-Z)
    led_g_state=$(echo "\$X52_LED_${led_ident}_green_${led_g_state}" | tr a-z A-Z)

    expect_pattern \
        $X52_LED_COMMAND_INDEX `eval echo $led_r_state` \
        $X52_LED_COMMAND_INDEX `eval echo $led_g_state`

    $X52CLI led $led_ident $led_color

    verify_output
}

# Run Mono-color LED tests
for led in fire throttle
do
    for state in on off
    do
        mono_led_test $led $state
    done
done

# Run multi-color LED tests
for led in a b d e t1 t2 t3 pov clutch
do
    for color in off red amber green
    do
        color_led_test $led $color
    done
done

verify_test_suite
