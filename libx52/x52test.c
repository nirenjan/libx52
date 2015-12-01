#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "libx52.h"

typedef void (*test_func)(libx52_device *dev, int step);

/* Run test initializations */
static void test_cleanup(libx52_device *dev)
{
    libx52_set_brightness(dev, 1, 127);
    libx52_set_brightness(dev, 0, 127);

    /* Set the default LED states */
    libx52_set_led_state(dev, LIBX52_LED_FIRE, LIBX52_LED_STATE_ON);
    libx52_set_led_state(dev, LIBX52_LED_A, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_B, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_D, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_E, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T1, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T2, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_T3, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_POV, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_CLUTCH, LIBX52_LED_STATE_GREEN);
    libx52_set_led_state(dev, LIBX52_LED_THROTTLE, LIBX52_LED_STATE_ON);

    /* Shift/Blink Off */
    libx52_set_shift(dev, 0);
    libx52_set_blink(dev, 0);

    /* Default Text */
    libx52_set_text(dev, 0, " Saitek X52 Pro", 15);
    libx52_set_text(dev, 1, "     Flight    ", 15);
    libx52_set_text(dev, 2, " Control System", 15);
}

static void test_mfd_display(libx52_device *dev, int step)
{
    uint32_t j;
    char str[16];
    char c;

    libx52_set_text(dev, 1, "0123456789ABCDEF", 16);

    /* Print line 0 - the hex of the step number */
    c = step;
    c += (c < 10) ? 0x30 : 0x37;
    for (j = 0; j < 16; j++) {
        str[j] = c;
    }
    libx52_set_text(dev, 0, str, 16);

    for (j = 0; j < 16; j++) {
        str[j] = (step << 4) + j;
    }
    libx52_set_text(dev, 2, str, 16);
}

/* Test the LEDs on the panel */
static void test_leds(libx52_device *dev, int step)
{
    int led;
    const char *state_map[] = {
        NULL,
        "A",
        "B",
        "D",
        "E",
        "T1",
        "T2",
        "T3",
        "POV",
        "I"
    };

    switch (step) {
    case 0:
        /* Turn off Fire LED */
        libx52_set_text(dev, 0, "LED Test", 8);
        libx52_set_text(dev, 1, "Fire", 4);
        libx52_set_text(dev, 2, "OFF", 3);
        libx52_set_led_state(dev, LIBX52_LED_FIRE, LIBX52_LED_STATE_OFF);
        break;

    case 1:
        /* Turn on Fire LED */
        libx52_set_text(dev, 1, "Fire", 4);
        libx52_set_text(dev, 2, "ON", 2);
        libx52_set_led_state(dev, LIBX52_LED_FIRE, LIBX52_LED_STATE_ON);
        break;

    case 2:
        /* Turn off Throttle LED */
        libx52_set_text(dev, 1, "Throttle", 8);
        libx52_set_text(dev, 2, "OFF", 3);
        libx52_set_led_state(dev, LIBX52_LED_THROTTLE, LIBX52_LED_STATE_OFF);
        break;

    case 3:
        /* Turn on Throttle LED */
        libx52_set_text(dev, 1, "Throttle", 8);
        libx52_set_text(dev, 2, "ON", 2);
        libx52_set_led_state(dev, LIBX52_LED_THROTTLE, LIBX52_LED_STATE_ON);
        break;

    default: {
        led = step / 4;
        libx52_set_text(dev, 1, state_map[led], strlen(state_map[led]));

        led = led * 2;
        switch (step % 4) {
        case 0:
            libx52_set_text(dev, 2, "OFF", 3);
            libx52_set_led_state(dev, led, LIBX52_LED_STATE_OFF);
            break;

        case 1:
            libx52_set_text(dev, 2, "RED", 3);
            libx52_set_led_state(dev, led, LIBX52_LED_STATE_RED);
            break;

        case 2:
            libx52_set_text(dev, 2, "AMBER", 5);
            libx52_set_led_state(dev, led, LIBX52_LED_STATE_AMBER);
            break;

        case 3:
            libx52_set_text(dev, 2, "GREEN", 5);
            libx52_set_led_state(dev, led, LIBX52_LED_STATE_GREEN);
            break;
        }
        }
    }
}

static void test_blinkenlichts(libx52_device *dev, int step)
{
    switch (step) {
    case 0:
        libx52_set_text(dev, 0, "Blinkenlichts", 13);
        libx52_set_text(dev, 1, "Shift Indicator", 15);
        libx52_set_text(dev, 2, "ON", 2);
        libx52_set_shift(dev, 1);
        break;

    case 1:
        libx52_set_text(dev, 2, "OFF", 3);
        libx52_set_shift(dev, 0);
        break;

    case 2:
        libx52_set_text(dev, 1, "Blink Indicator", 15);
        libx52_set_text(dev, 2, "ON", 2);
        libx52_set_blink(dev, 1);
        break;

    case 3:
        libx52_set_text(dev, 2, "OFF", 3);
        libx52_set_blink(dev, 0);
        break;
    }
}

static void test_brightness(libx52_device *dev, int step)
{
    char line1[16];
    char line2[16];
    int len1;
    int len2;

    if (step == 0) {
        libx52_set_text(dev, 0, "Brightness", 10);
    }

    len1 = snprintf(line1, sizeof(line1), "MFD: %d", step);
    len2 = snprintf(line2, sizeof(line2), "LED: %d", step);

    libx52_set_text(dev, 1, line1, len1);
    libx52_set_text(dev, 2, line2, len2);
    libx52_set_brightness(dev, 0, step);
    libx52_set_brightness(dev, 1, step);
}

struct test_case {
    test_func   test;
    int         test_steps;
};

static struct test_case cases[] = {
    { test_brightness, 129 },
    { test_mfd_display, 16 },
    { test_leds, 40 },
    { test_blinkenlichts, 4 },
    { NULL, 0 } /* Must be the last entry */
};

int main()
{
    uint32_t i;
    uint32_t j;
    char str[16];
    char c;
    struct test_case *tc;

    libx52_device *dev;

    dev = libx52_init();

    tc = &cases[0];
    do {
        for (i = 0; i < tc->test_steps; i++) {
            if (tc->test) {
                (*tc->test)(dev, i);
            }
            libx52_update(dev);
            usleep(500000);
        }

        tc++;

        /* Reset the X52 */
        test_cleanup(dev);
        libx52_update(dev);
    } while (tc->test);

    libx52_exit(dev);
    return 0;
}
