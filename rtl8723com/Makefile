# SPDX-License-Identifier: GPL-2.0
rtl8723-common-objs :=		\
		main.o		\
		dm_common.o	\
		fw_common.o	\
		phy_common.o

obj-m += rtl8723-common.o

CC = gcc
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
PWD := $(shell pwd)

EXTRA_CFLAGS += -O2
#EXTRA_CFLAGS += -DRTK_FW_HDR

####################################################
obj-m := rtl8723com.o


PCI_8723BE_COM_OBJS:=	\
	main.o		\
	dm_common.o	\
	fw_common.o	\
	phy_common.o

####################################################

rtl8723com-objs += $(PCI_8723BE_COM_OBJS)


default:
	        $(MAKE) -C $(KSRC) M=$(PWD) modules

.PHONY:clean
clean:
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
	rm -fr Modules.symvers
	rm -fr Module.symvers
	rm -fr Module.markers
	rm -fr modules.order
	rm -fr tags

