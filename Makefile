obj-m = lxdriver.o
lxdriver-objs = lx_acc_driver.o lis3dh_acc.o

KDIR := linux-headers-3.8.13-bone70
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

all: lxaccelldriver

lxaccelldriver:
	make -C $(KDIR) M=${shell pwd} modules ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

clean:
	make -C $(KDIR) M=${shell pwd} clean