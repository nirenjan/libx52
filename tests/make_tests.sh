#!/bin/bash
# Script to generate the test scripts
# Most of the test cases are extremely similar, but they do need to be created
# and saved in the repository so that the automake infrastructure can pick them
# up. 

# Common header for each test script
_test_header()
{
cat << EOF
#!/bin/bash
# $@

source \$(dirname \$0)/../common_infra.sh
EOF
}

# Template for single-color LEDs
_mono_led_template()
{
    local led_ident=$(echo $1 | tr a-z A-Z)
    local led_color=$2
    local state=$(echo "\$X52_LED_${led_ident}_${led_color}" | tr a-z A-Z)
cat << EOF
$(_test_header Test setting the $led_ident button $led_color)

expect_pattern \$X52_LED_COMMAND_INDEX $state

\$X52CLI led $led_ident $led_color

verify_output

EOF
}

# Template for multi-color LEDs
_color_led_template()
{
    local led_ident=$(echo $1 | tr a-z A-Z)
    local led_color=$2
    local led_r_state=off
    local led_g_state=off

    case $led_color in
    "red")
        led_r_state=on
        ;;
    "green")
        led_g_state=on
        ;;
    "amber")
        led_r_state=on
        led_g_state=on
        ;;
    esac

    led_r_state=$(echo "\$X52_LED_${led_ident}_red_${led_r_state}" | tr a-z A-Z)
    led_g_state=$(echo "\$X52_LED_${led_ident}_green_${led_g_state}" | tr a-z A-Z)

cat << EOF
$(_test_header Test setting the $led_ident button to $led_color)

expect_pattern \\
    \$X52_LED_COMMAND_INDEX $led_r_state \\
    \$X52_LED_COMMAND_INDEX $led_g_state

\$X52CLI led $led_ident $led_color

verify_output

EOF
}

# Function to generate LED test cases
make_led_tests()
{
    mkdir -p led

    # Make the mono-color LED tests
    for led in fire throttle
    do
        for state in off on
        do
            filename=led/test_led_${led}_${state}.sh
            _mono_led_template $led $state > $filename
            echo -e "\t$filename \\" >> Makefile.am
        done
    done

    # Make the multi-color LED tests
    for led in a b d e t1 t2 t3 pov clutch
    do
        for state in off red amber green
        do
            filename=led/test_led_${led}_${state}.sh
            _color_led_template $led $state > $filename
            echo -e "\t$filename \\" >> Makefile.am
        done
    done
}

# Template for brightness test cases
_brightness_template()
{
    local unit=$(echo $1 | tr a-z A-Z)
    local bri=$(printf '0x%04x' $2)
    local index="\$X52_${unit}_BRIGHTNESS_INDEX"

cat << EOF
$(_test_header Test setting $unit brightness to $bri)

expect_pattern $index $bri

\$X52CLI bri $unit $bri

verify_output

EOF
}

# Function to generate brightness test cases
make_brightness_tests()
{
    mkdir -p brightness

    for unit in mfd led
    do
        for bri in $(seq 0 128)
        do
            bri_fn=$(printf '%02x' $bri)
            filename=brightness/test_brightness_${unit}_${bri_fn}.sh
            _brightness_template $unit $bri > $filename
            echo -e "\t$filename \\" >> Makefile.am
        done
    done
}

# Function to setup Makefile.am to receive the generated test cases
clear_tests()
{
    # Delete the tests from Makefile.am
    sed -i '/^TESTS /,$d' Makefile.am
    echo "TESTS = \\" >> Makefile.am
}

# Function to ensure that Makefile.am works correctly with both
# make check and make distcheck
finalize_tests()
{
    # Put the last line to close the tests list
    echo -e "\ttest_skip.sh\n" >> Makefile.am

    echo -e "\nEXTRA_DIST = common_infra.sh \$(TESTS)\n" >> Makefile.am
}

clear_tests
make_led_tests
make_brightness_tests
finalize_tests
