SRCS := $(wildcard *.c)
HEADERS := $(wildcard *.h)

test_x52: ${SRCS} ${HEADERS}
	gcc -I /usr/include/libusb-1.0/ ${SRCS} -lusb-1.0 -o $@

.PHONY: clean
clean:
	rm -f test_x52 *.o
