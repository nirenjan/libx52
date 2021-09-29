obj-m := saitek_x52.o saitek_x65.o
saitek_x52-objs := hid-saitek-x52.o
saitek_x65-objs := hid-saitek-x65.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
