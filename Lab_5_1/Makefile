ifneq ($(KERNELRELEASE),)
	obj-m := hello_zed.o 
else
	KERNELDIR ?= /home/socqqq123/Desktop/zynq/linux-xlnx
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

help:
