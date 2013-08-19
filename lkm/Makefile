col_kl : col_kl.c
		cc -o col_kl col_kl.c

ifneq ($(KERNELRELEASE),)
	obj-m := rootkit.o
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif
