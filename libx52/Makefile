
test_x52: x52control.c x52control.h
	gcc -I /usr/include/libusb-1.0/ x52control.c -lusb-1.0 -o $@

.PHONY: clean
clean:
	rm -f test_x52 *.o
