#!/bin/bash
# Clock tests
#
# Copyright (C) 2012-2018 Nirenjan Krishnan (nirenjan@nirenjan.org)
#
# SPDX-License-Identifier: GPL-2.0-only WITH Classpath-exception-2.0

source $(dirname $0)/../common_infra.sh

TEST_SUITE_ID="libx52 clock tests"

clock_test_utc()
{
    local timezone=$1
    local time_format=$2
    local date_format=$3
    TEST_ID="Test setting clock to $timezone time, $time_format, $date_format"

    local dd=$(date +%_d)
    local mm=$(date +%_m)
    local yy=$(date +%_y)
    local hr=$(date +%_H)
    local mn=$(date +%_M)

    local clock_value=$(($hr * 256 + $mn))
    if [[ $time_format == 24hr ]]
    then
        clock_value=$(($clock_value + 32768))
    fi

    case $date_format in
    ddmmyy)
        date_value=$(($mm * 256 + $dd))
        year_value=$yy
        ;;
    mmddyy)
        date_value=$(($dd * 256 + $mm))
        year_value=$yy
        ;;
    yymmdd)
        date_value=$(($mm * 256 + $yy))
        year_value=$dd
        ;;
    *)
        ;;
    esac

    expect_pattern \
        $X52_CLOCK_DATE_INDEX $(printf '%04x' $date_value) \
        $X52_CLOCK_YEAR_INDEX $(printf '%04x' $year_value) \
        $X52_CLOCK_1_INDEX $(printf '%04x' $clock_value) \

    $X52CLI clock $timezone $time_format $date_format

    verify_output
}

offset_test()
{
    local clock_id=$1
    local offset=$2
    local time_format=$3
    TEST_ID="Test setting clock $clock_id offset to $offset, $time_format"

    local index="\$X52_CLOCK_${clock_id}_INDEX"
    local value=0

    # Handle negative case
    if [[ $offset -lt 0 ]]
    then
        value=$((-$offset))
        value=$(($value & 0x3ff))
        value=$(($value | 1024))
    else
        value=$offset
    fi

    if [[ $time_format == 24hr ]]
    then
        value=$(($value + 32768))
    fi

    expect_pattern $(eval echo $index) $(printf '%04x' $value)

    $X52CLI offset $clock_id $offset $time_format

    verify_output

}

raw_time_test()
{
    local hh=$1
    local mm=$2
    local time_format=$3
    TEST_ID="Test setting time to $hh:$mm, $time_format"

    local value=$(($hh * 256 + $mm))
    if [[ $time_format == 24hr ]]
    then
        value=$((value + 32768))
    fi

    expect_pattern $X52_CLOCK_1_INDEX $(printf '%04x' $value)

    $X52CLI time $hh $mm $time_format

    verify_output
}

raw_date_test()
{
    local dd=$1
    local mm=$2
    local yy=$3
    local date_format=$4

    case "$date_format" in
    ddmmyy)
        dd1=$dd
        mm1=$mm
        yy1=$yy
        ;;
    mmddyy)
        dd1=$mm
        mm1=$dd
        yy1=$yy
        ;;
    yymmdd)
        dd1=$yy
        mm1=$mm
        yy1=$dd
        ;;
    esac

    TEST_ID="Test setting date in $date_format format to ${dd1}-${mm1}-${yy1}"

    local date_value=$(printf '%04x' $(($mm1 * 256 + $dd1)))
    local year_value=$(printf '%04x' $yy1)

    expect_pattern \
        $X52_CLOCK_DATE_INDEX $date_value \
        $X52_CLOCK_YEAR_INDEX $year_value

    $X52CLI date $dd $mm $yy $date_format

    verify_output
}

# For all test cases, we want to assume that the local timezone
# is the same as UTC, as otherwise, it can cause havoc, depending
# on when and where the tests are run.
export TZ=UTC

for timezone in 'local' gmt
do
    for time_format in 12hr 24hr
    do
        for date_format in ddmmyy mmddyy yymmdd
        do
            clock_test_utc $timezone $time_format $date_format
        done
    done
done

# For the offset test cases, x52cli reinitializes the internal data
# structure every time it is called, therefore, the timezone for clock
# 1 is automatically reset to UTC.
unset TZ

for clock in 2 3
do
    for time_format in 12hr 24hr
    do
        # Run offset tests for every 30 minute offset from UTC
        for offset in $(seq -1020 30 1020)
        do
            offset_test $clock $offset $time_format
        done
    done
done

# The raw time test cases are a limited set of tests, it simply runs for
# every minute from 00:00 through 00:59, then it runs for every hour from
# 00:00 through 23:00. The tests are run in both 12 hour and 24 hour mode.
for mm in $(seq 0 59)
do
    for time_format in 12hr 24hr
    do
        raw_time_test 0 $mm $time_format
    done
done

for hh in $(seq 0 23)
do
    for time_format in 12hr 24hr
    do
        raw_time_test $hh 0 $time_format
    done
done

# The raw date tests simply verify that the date February 1, year 3 is
# displayed correctly in all 3 date formats
for date_format in ddmmyy mmddyy yymmdd
do
    raw_date_test 1 2 3 $date_format
done

verify_test_suite

