
CC = gcc
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
PWD := $(shell pwd)

EXTRA_CFLAGS += -O2
#EXTRA_CFLAGS += -DRTK_FW_HDR

####################################################
obj-m := rtl8723ae.o


PCI_8723AE_HAL_OBJS:=	\
	dm.o		\
	fw.o		\
	hal_btc.o	\
	hal_bt_coexist.o\
	hw.o		\
	led.o		\
	phy.o		\
	pwrseq.o	\
	pwrseqcmd.o	\
	rf.o		\
	sw.o		\
	table.o		\
	trx.o		\

####################################################

rtl8723ae-objs += $(PCI_8723AE_HAL_OBJS)

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

