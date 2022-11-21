##
# Project Title
#
# @file
# @version 0.1
#

obj-m += qndkm.o
qndkm-objs := quantum_network_device_kernel_module.o
qndkm-objs += ./src/register.o
qndkm-objs += ./src/echo.o
qndkm-objs += ./src/command.o

#c99 standard
ccflags-y := -std=gnu99

# add our custom header file
EXTRA_CFLAGS=-I$(PWD)/include/
TARGET = quntum_network_device.ko

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean



# end
