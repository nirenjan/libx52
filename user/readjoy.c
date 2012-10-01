#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>

int main(int argc, char *argv[])
{
    struct js_event joy_event;
    int fd;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s /dev/input/jsX\n", argv[0]);
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (!fd) {
        fprintf(stderr, "Error: Unable to open %s!\n", argv[1]);
        return -2;
    }

    for(;;) {
        if (read(fd, &joy_event, sizeof(joy_event)) != sizeof(joy_event)) {
            fprintf(stderr, "Error: Unable to read from joystick!\n");
            return -3;
        } else {
            if (joy_event.type & JS_EVENT_INIT) {
                printf("*");
            }

            if (joy_event.type & JS_EVENT_BUTTON) {
                printf("B");
            } else if (joy_event.type & JS_EVENT_AXIS) {
                printf("A");
            } else {
                printf("?");
            }
            printf("%u: %d\n", joy_event.number, joy_event.value);
        }
    }

    return 0;
}
