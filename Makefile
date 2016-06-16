obj-m = lxdriver.o
KDIR := linux-headers-3.8.13-bone70

all:
	make -C $(KDIR) M=${shell pwd} modules

clean:
	make -C $(KDIR) M=${shell pwd} clean