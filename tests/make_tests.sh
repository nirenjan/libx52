#!/bin/bash
# Script to generate the test scripts
# Most of the test cases are extremely similar, but they do need to be created
# and saved in the repository so that the automake infrastructure can pick them
# up. 

_mono_led_template()
{
    local led_ident=$(echo $1 | tr a-z A-Z)
    local led_color=$2
    local state=$(echo "\$X52_LED_${led_ident}_${led_color}" | tr a-z A-Z)
cat << EOF
#!/bin/bash
# Test setting the $led_ident button $led_color

source \$(dirname \$0)/common_infra.sh

expect_pattern \$X52_LED_COMMAND_INDEX $state

\$X52CLI led $led_ident $led_color

verify_output

EOF
}

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
#!/bin/bash
# Test setting the $led_ident button to $led_color

source \$(dirname \$0)/common_infra.sh

expect_pattern \\
    \$X52_LED_COMMAND_INDEX $led_r_state \\
    \$X52_LED_COMMAND_INDEX $led_g_state

\$X52CLI led $led_ident $led_color

verify_output

EOF
}

make_led_tests()
{
    # Make the mono-color LED tests
    for led in fire throttle
    do
        for state in off on
        do
            filename=test_led_${led}_${state}.sh
            _mono_led_template $led $state > $filename
            echo -e "\t$filename \\" >> Makefile.am
        done
    done

    # Make the multi-color LED tests
    for led in a b d e t1 t2 t3 pov clutch
    do
        for state in off red amber green
        do
            filename=test_led_${led}_${state}.sh
            _color_led_template $led $state > test_led_${led}_${state}.sh
            echo -e "\t$filename \\" >> Makefile.am
        done
    done
}

clear_tests()
{
    # Delete the tests from Makefile.am
    sed -i '/^TESTS /,$d' Makefile.am
    echo "TESTS = \\" >> Makefile.am
}

finalize_tests()
{
    # Put the last line to close the tests list
    echo -e "\ttest_skip.sh\n" >> Makefile.am

    echo -e "\nEXTRA_DIST = common_infra.sh \$(TESTS)\n" >> Makefile.am
}

clear_tests
make_led_tests
finalize_tests
