# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
#
# Comment/uncomment the following line to diable/enable
# debugging
#
DEBUG = y

#Add your debugging flag (or not) to CFLAGS

ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DSCULL_DEBUG # "-0" is needed to expand inlines
else
	DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)
	obj-m :=scull.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.

else

	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

