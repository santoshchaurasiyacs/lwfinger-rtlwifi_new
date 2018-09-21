# SPDX-License-Identifier: GPL-2.0
SHELL := /bin/sh
CC = gcc
KVER  := $(shell uname -r)
#KVER  :=
KSRC := /lib/modules/$(KVER)/build
#KSRC := /work/linux-src/linux-stable
FIRMWAREDIR := /lib/firmware/
PWD := $(shell pwd)
CLR_MODULE_FILES := *.mod.c *.mod *.o .*.cmd *.ko *~ .tmp_versions* modules.order Module.symvers
SYMBOL_FILE := Module.symvers
# Handle the move of the entire rtlwifi tree
ifneq ("","$(wildcard /lib/modules/$(KVER)/kernel/drivers/net/wireless/realtek)")
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/realtek/rtlwifi
else
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/rtlwifi
endif

#Handle the compression option for modules in 3.18+
ifneq ("","$(wildcard $(MODDESTDIR)/*.ko.gz)")
COMPRESS_GZIP := y
endif
ifneq ("","$(wildcard $(MODDESTDIR)/*.ko.xz)")
COMPRESS_XZ := y
endif

ccflags-y += -O2

obj-m		+= rtwlan.o
rtwlan-objs += main.o \
	    mac80211.o \
	    debug.o \
	    tx.o \
	    rx.o \
	    mac.o \
	    phy.o \
	    efuse.o \
	    fw.o \
	    ps.o \
	    sec.o \
	    regd.o \
	    rtw8822b.o rtw8822b_table.o \
	    rtw8822c.o rtw8822c_table.o \
	    pci.o
ccflags-y += -D__CHECK_ENDIAN__
#subdir-ccflags-y += -Werror
ccflags-y += -DCONFIG_RTLWIFI_DEBUG
subdir-ccflags-y += -DCONFIG_RTLWIFI_DEBUG

ifeq ("$(KVER)", $(filter "$(KVER)", "3.14.35-031435-generic" "3.14.35-031435-lowlatency"))
ccflags-y += -D_ieee80211_is_robust_mgmt_frame=ieee80211_is_robust_mgmt_frame
subdir-ccflags-y += -D_ieee80211_is_robust_mgmt_frame=ieee80211_is_robust_mgmt_frame
endif

CHECKFLAGS += -D__CHECK_ENDIAN__

all:
	$(MAKE) -C $(KSRC) M=$(PWD) modules
install: all
	@mkdir -p $(MODDESTDIR)/rtwlan
ifeq ($(COMPRESS_GZIP), y)
	@gzip -f $(MODDESTDIR)/*.ko
endif
ifeq ($(COMPRESS_XZ), y)
	@xz -f $(MODDESTDIR)/*.ko
endif

	@depmod -a

uninstall:
	@echo "Uninstall rtlwifi SUCCESS"

clean:
	@rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~ .cache.mk
	@rm -fr .tmp_versions
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order
