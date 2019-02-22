SHELL := /bin/sh
CC = gcc
KVER  ?= $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
IRMWAREDIR := /lib/firmware/
PWD := $(shell pwd)
CLR_MODULE_FILES := *.mod.c *.mod *.o .*.cmd *.ko *~ .tmp_versions* modules.order Module.symvers
SYMBOL_FILE := Module.symvers
# Handle the move of the entire rtw88 tree
ifneq ("","$(wildcard /lib/modules/$(KVER)/kernel/drivers/net/wireless/realtek)")
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/realtek/rtw88
else
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/rtw88
endif
#Handle the compression option for modules in 3.18+
ifneq ("","$(wildcard $(MODDESTDIR)/*.ko.gz)")
COMPRESS_GZIP := y
endif
ifneq ("","$(wildcard $(MODDESTDIR)/*.ko.xz)")
COMPRESS_XZ := y
endif

EXTRA_CFLAGS += -O2

obj-m	+= rtw88.o
rtw88-objs += main.o \
	   mac80211.o \
	   util.o \
	   debug.o \
	   tx.o \
	   rx.o \
	   mac.o \
	   phy.o \
	   efuse.o \
	   fw.o \
	   ps.o \
	   sec.o \
	   regd.o

rtw88-objs	+= rtw8822b.o rtw8822b_table.o
rtw88-objs	+= rtw8822c.o rtw8822c_table.o

obj-m		+= rtwpci.o
rtwpci-objs			:= pci.o

ccflags-y += -D__CHECK_ENDIAN__

all: 
	$(MAKE) -C $(KSRC) M=$(PWD) modules
install: all
ifeq (,$(wildcard ./backup_drivers.tar))
	@echo Making backups
	@tar cPf backup_drivers.tar $(MODDESTDIR)
endif

	@mkdir -p $(MODDESTDIR)
	@install -p -D -m 644 rtwpci.ko $(MODDESTDIR)	
	@install -p -D -m 644 rtw88.ko $(MODDESTDIR)	
ifeq ($(COMPRESS_GZIP), y)
	@gzip -f $(MODDESTDIR)/*.ko
endif
ifeq ($(COMPRESS_XZ), y)
	@xz -f $(MODDESTDIR)/*.ko
endif

	@depmod -a $(KVER)

	@#copy firmware images to target folder
	@mkdir -p $(FIRMWAREDIR)/rtw88/
	@cp -f firmware/*.bin $(FIRMWAREDIR)/rtw88/
	@echo "Install rtw88 SUCCESS"

uninstall:
ifneq (,$(wildcard ./backup_drivers.tar))
	@echo Restoring backups
	@tar xvPf backup_drivers.tar
endif
	
	@depmod -a
	
	@echo "Uninstall rtw88 SUCCESS"

clean:
	@rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~ .*.o.d .cache.mk
	@rm -fr .tmp_versions
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order
