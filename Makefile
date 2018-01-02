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

CONFIG_RTLWIFI=m
CONFIG_RTLWIFI_PCI=m
CONFIG_RTLWIFI_USB=m
CONFIG_RTL8192C_COMMON=m
CONFIG_RTLWIFI_DEBUG=y
CONFIG_RTL8192CE=m
CONFIG_RTL8192CU=m
CONFIG_RTL8192SE=m
CONFIG_RTL8192DE=m
CONFIG_RTL8723AE=m
CONFIG_RTL8723BE=m
CONFIG_RTL8723DE=m
CONFIG_RTL8188EE=m
CONFIG_RTLBTCOEXIST=m
CONFIG_RTLHALMAC=m
CONFIG_RTLPHYDM=m
CONFIG_RTL8723_COMMON=m
CONFIG_RTL8821AE=m
CONFIG_RTL8822BE=m
CONFIG_RTL8192EE=m

export CONFIG_RTL8192C_COMMON CONFIG_RTL8192CE CONFIG_RTL8192CU CONFIG_RTL8192SE CONFIG_RTL8192DE CONFIG_RTL8192EE
export CONFIG_RTL8723AE CONFIG_RTL8723BE CONFIG_RTL8723DE CONFIG_RTL8723_COMMON
export CONFIG_RTL8188EE CONFIG_RTL8821AE
export CONFIG_RTLBTCOEXIST


obj-m		+= rtlwifi.o
rtlwifi-objs	:=		\
		base.o		\
		cam.o		\
		core.o		\
		debug.o		\
		efuse.o		\
		ps.o		\
		rc.o		\
		regd.o		\
		stats.o

rtl8192c_common-objs +=		\

obj-m				+= rtl_pci.o
rtl_pci-objs	:=		pci.o

obj-m				+= rtl_usb.o
rtl_usb-objs	:=		usb.o

obj-m				+= rtl8192c/
obj-m				+= rtl8192ce/
obj-m				+= rtl8192cu/
obj-m				+= rtl8192se/
obj-m				+= rtl8192de/
obj-m				+= rtl8723ae/
obj-m				+= rtl8723be/
obj-m				+= rtl8723de/
obj-m				+= rtl8188ee/
obj-m				+= btcoexist/
obj-m				+= halmac/
obj-m				+= phydm/
obj-m				+= rtl8723com/
obj-m				+= rtl8821ae/
obj-m				+= rtl8822be/
obj-m				+= rtl8192ee/

ccflags-y += -D__CHECK_ENDIAN__
#subdir-ccflags-y += -Werror
ccflags-y += -DCONFIG_RTLWIFI_DEBUG
subdir-ccflags-y += -DCONFIG_RTLWIFI_DEBUG

ifeq ("$(KVER)", $(filter "$(KVER)", "3.14.35-031435-generic" "3.14.35-031435-lowlatency"))
ccflags-y += -D_ieee80211_is_robust_mgmt_frame=ieee80211_is_robust_mgmt_frame
subdir-ccflags-y += -D_ieee80211_is_robust_mgmt_frame=ieee80211_is_robust_mgmt_frame
endif

# for uncooked code
uncooked_ccflags-y += -DBT_SUPPORT=1
uncooked_ccflags-y += -DCOEX_SUPPORT=1
uncooked_ccflags-y += -DRTL8822B_SUPPORT=1
uncooked_ccflags-y += -DRTL8723D_SUPPORT=1
ccflags-y += $(uncooked_ccflags-y)
subdir-ccflags-y += $(uncooked_ccflags-y)

CHECKFLAGS += -D__CHECK_ENDIAN__

all:
	$(MAKE) -C $(KSRC) M=$(PWD) modules
install: all
ifeq (,$(wildcard ./backup_drivers.tar))
	@echo Making backups
	@tar cPf backup_drivers.tar $(MODDESTDIR)
endif

	@mkdir -p $(MODDESTDIR)/btcoexist
	@mkdir -p $(MODDESTDIR)/halmac
	@mkdir -p $(MODDESTDIR)/phydm
	@mkdir -p $(MODDESTDIR)/rtl8188ee
	@mkdir -p $(MODDESTDIR)/rtl8192c
	@mkdir -p $(MODDESTDIR)/rtl8192ce
	@mkdir -p $(MODDESTDIR)/rtl8192cu
	@mkdir -p $(MODDESTDIR)/rtl8192de
	@mkdir -p $(MODDESTDIR)/rtl8192ee
	@mkdir -p $(MODDESTDIR)/rtl8192se
	@mkdir -p $(MODDESTDIR)/rtl8723ae
	@mkdir -p $(MODDESTDIR)/rtl8723be
	@mkdir -p $(MODDESTDIR)/rtl8723de
	@mkdir -p $(MODDESTDIR)/rtl8723com
	@mkdir -p $(MODDESTDIR)/rtl8821ae
	@mkdir -p $(MODDESTDIR)/rtl8822be
	@install -p -D -m 644 rtl_pci.ko $(MODDESTDIR)
	@install -p -D -m 644 rtl_usb.ko $(MODDESTDIR)
	@install -p -D -m 644 rtlwifi.ko $(MODDESTDIR)
	@install -p -D -m 644 ./btcoexist/btcoexist.ko $(MODDESTDIR)/btcoexist
	@install -p -D -m 644 ./halmac/halmac.ko $(MODDESTDIR)/halmac
	@install -p -D -m 644 ./phydm/phydm_mod.ko $(MODDESTDIR)/phydm
	@install -p -D -m 644 ./rtl8188ee/rtl8188ee.ko $(MODDESTDIR)/rtl8188ee
	@install -p -D -m 644 ./rtl8192c/rtl8192c-common.ko $(MODDESTDIR)/rtl8192c
	@install -p -D -m 644 ./rtl8192ce/rtl8192ce.ko $(MODDESTDIR)/rtl8192ce
	@install -p -D -m 644 ./rtl8192cu/rtl8192cu.ko $(MODDESTDIR)/rtl8192cu
	@install -p -D -m 644 ./rtl8192de/rtl8192de.ko $(MODDESTDIR)/rtl8192de
	@install -p -D -m 644 ./rtl8192ee/rtl8192ee.ko $(MODDESTDIR)/rtl8192ee
	@install -p -D -m 644 ./rtl8192se/rtl8192se.ko $(MODDESTDIR)/rtl8192se
	@install -p -D -m 644 ./rtl8723ae/rtl8723ae.ko $(MODDESTDIR)/rtl8723ae
	@install -p -D -m 644 ./rtl8723be/rtl8723be.ko $(MODDESTDIR)/rtl8723be
	@install -p -D -m 644 ./rtl8723de/rtl8723de.ko $(MODDESTDIR)/rtl8723de
	@install -p -D -m 644 ./rtl8821ae/rtl8821ae.ko $(MODDESTDIR)/rtl8821ae
	@install -p -D -m 644 ./rtl8822be/rtl8822be.ko $(MODDESTDIR)/rtl8822be
	@install -p -D -m 644 ./rtl8723com/rtl8723-common.ko $(MODDESTDIR)/rtl8723com
ifeq ($(COMPRESS_GZIP), y)
	@gzip -f $(MODDESTDIR)/*.ko
	@gzip -f $(MODDESTDIR)/btcoexist/*.ko
	@gzip -f $(MODDESTDIR)/rtl8*/*.ko
	@gzip -f $(MODDESTDIR)/halmac/*.ko
	@gzip -f $(MODDESTDIR)/phydm/*.ko
endif
ifeq ($(COMPRESS_XZ), y)
	@xz -f $(MODDESTDIR)/*.ko
	@xz -f $(MODDESTDIR)/btcoexist/*.ko
	@xz -f $(MODDESTDIR)/rtl8*/*.ko
	@xz -f $(MODDESTDIR)/halmac/*.ko
	@xz -f $(MODDESTDIR)/phydm/*.ko
endif

	@depmod -a

	@#copy firmware images to target folder
	@cp -fr firmware/rtlwifi/ $(FIRMWAREDIR)/
	@echo "Install rtlwifi SUCCESS"

uninstall:
ifneq (,$(wildcard ./backup_drivers.tar))
	@echo Restoring backups
	@tar xvPf backup_drivers.tar
endif

	@depmod -a

	@echo "Uninstall rtlwifi SUCCESS"

clean:
	@find halmac/ \( -name "*.mod.c" -o -name "*.mod" -o -name "*.o" -o -name ".*.cmd" -o -name "*.ko" -o -name "*~" \) -exec rm {} \;
	@find phydm/ \( -name "*.mod.c" -o -name "*.mod" -o -name "*.o" -o -name ".*.cmd" -o -name "*.ko" -o -name "*~" \) -exec rm {} \;
	@rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	@rm -fr rtl8*/*.mod.c rtl8*/*.mod rtl8*/*.o rtl8*/.*.cmd rtl8*/*.ko rtl8*/*~
	@rm -fr bt*/*.mod.c bt*/*.mod bt*/*.o bt*/.*.cmd bt*/*.ko bt*/*~
	@rm -fr .tmp_versions
	@rm -fr Modules.symvers
	@rm -fr Module.symvers
	@rm -fr Module.markers
	@rm -fr modules.order
