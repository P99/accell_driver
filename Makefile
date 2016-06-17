obj-m = lxdriver.o
KDIR := linux-headers-3.8.13-bone70

all: lxaccelldriver

lxaccelldriver:
	make -C $(KDIR) M=${shell pwd} modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

clean:
	make -C $(KDIR) M=${shell pwd} clean