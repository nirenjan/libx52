#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "libx52.h"

int main()
{
    uint32_t i;
    uint32_t j;
    char str[16];
    char c; 

    libx52_device *dev;
    
    dev = libx52_init();

    libx52_set_brightness(dev, 1, 127);
    libx52_set_brightness(dev, 0, 127);

    libx52_set_led(dev, X52_LED_FIRE, 1);

    libx52_set_text(dev, 1, "0123456789ABCDEF", 16);

    for (i = 0; i < 256; i+=16) {
        c = i >> 4;
        c += (c < 10) ? 0x30 : 0x37;
        for (j = 0; j < 16; j++) {
            str[j] = c;
        }
        libx52_set_text(dev, 0, str, 16);

        for (j = 0; j < 16; j++) {
            str[j] = i + j;
        }
        libx52_set_text(dev, 2, str, 16);
        libx52_update(dev);
        sleep(1);
    }

    libx52_set_text(dev, 0, " Saitek X52 Pro ", 16);
    libx52_set_text(dev, 1, "     Flight     ", 16);
    libx52_set_text(dev, 2, " Control System ", 16);
    libx52_update(dev);

    libx52_exit(dev);
    return 0;
}
