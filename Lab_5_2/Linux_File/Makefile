
obj-m := ip.o
KERNELDIR ?= /home/socqqq123/Desktop/zynq/linux-xlnx

default:
	ARCH=arm
	CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

help:

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
