obj-m := saitek_x52.o
saitek_x52-objs := x52joy.o x52joy_commands.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
