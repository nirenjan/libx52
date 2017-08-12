#!/bin/bash

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
verify_test_suite

