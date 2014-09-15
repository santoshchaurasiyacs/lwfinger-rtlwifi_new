
CC = gcc
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/rtlwifi
FIRMWAREDIR := /lib/firmware/
PWD := $(shell pwd)
CLR_MODULE_FILES := *.mod.c *.mod *.o .*.cmd *.ko *~ .tmp_versions* modules.order Module.symvers
SYMBOL_FILE := Module.symvers

EXTRA_CFLAGS += -O2
obj-m := rtlwifi.o
PCI_MAIN_OBJS	:=	\
		base.o	\
		cam.o	\
		core.o	\
		debug.o	\
		efuse.o	\
		ps.o	\
		rc.o	\
		regd.o	\
		stats.o

obj-m	+= rtl_pci.o
rtl_pci-objs	:=		pci.o

obj-m	+= rtl_usb.o
rtl_usb-objs	:=		usb.o

rtlwifi-objs += $(PCI_MAIN_OBJS)

ccflags-y += -D__CHECK_ENDIAN__

all: 
	$(MAKE) -C $(KSRC) M=$(PWD) modules
	@cp $(SYMBOL_FILE) btcoexist/
	+@make -C btcoexist/
	@cp $(SYMBOL_FILE) rtl8188ee/
	+@make -C rtl8188ee/
	@cp $(SYMBOL_FILE) rtl8192c/
	+@make -C rtl8192c/
	@cp $(SYMBOL_FILE) rtl8192ce/
	@cp rtl8192c/$(SYMBOL_FILE) rtl8192ce/
	+@make -C rtl8192ce/
	@cp $(SYMBOL_FILE) rtl8192cu/
	@cp rtl8192c/$(SYMBOL_FILE) rtl8192cu/
	+@make -C rtl8192cu/
	@cp $(SYMBOL_FILE) rtl8192de/
	+@make -C rtl8192de/
	@cp $(SYMBOL_FILE) rtl8192ee/
	@cp btcoexist/$(SYMBOL_FILE) rtl8192ee/
	+@make -C rtl8192ee/
	@cp $(SYMBOL_FILE) rtl8192se/
	+@make -C rtl8192se/
	@cp $(SYMBOL_FILE) rtl8723ae/
	@cp btcoexist/$(SYMBOL_FILE) rtl8723ae/
	+@make -C rtl8723ae/
	@cp $(SYMBOL_FILE) rtl8723be/
	@cp btcoexist/$(SYMBOL_FILE) rtl8723be/
	+@make -C rtl8723be/
	@cp $(SYMBOL_FILE) rtl8821ae/
	@cp btcoexist/$(SYMBOL_FILE) rtl8821ae/
	+@make -C rtl8821ae/
install: all
	@find /lib/modules/$(shell uname -r) -name "rtl_pci.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl_usb.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtlwifi.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8192c_common.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "btcoexist.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8188ee.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8192ce.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8192de.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8192ee.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8192se.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8723be.ko" -exec rm {} \;
	@find /lib/modules/$(shell uname -r) -name "rtl8821ae.ko" -exec rm {} \;
	@rm -fr $(FIRMWAREDIR)/`uname -r`/rtlwifi

	$(shell rm -fr $(MODDESTDIR))
	$(shell mkdir $(MODDESTDIR))
	$(shell mkdir $(MODDESTDIR)/btcoexist)
	$(shell mkdir $(MODDESTDIR)/rtl8188ee)
	$(shell mkdir $(MODDESTDIR)/rtl8192c)
	$(shell mkdir $(MODDESTDIR)/rtl8192ce)
	$(shell mkdir $(MODDESTDIR)/rtl8192de)
	$(shell mkdir $(MODDESTDIR)/rtl8192ee)
	$(shell mkdir $(MODDESTDIR)/rtl8192se)
	$(shell mkdir $(MODDESTDIR)/rtl8723ae)
	$(shell mkdir $(MODDESTDIR)/rtl8723be)
	$(shell mkdir $(MODDESTDIR)/rtl8821ae)
	@install -p -m 644 rtl_pci.ko $(MODDESTDIR)	
	@install -p -m 644 rtl_usb.ko $(MODDESTDIR)	
	@install -p -m 644 rtlwifi.ko $(MODDESTDIR)	
	@install -p -m 644 ./btcoexist/btcoexist.ko $(MODDESTDIR)/btcoexist
	@install -p -m 644 ./rtl8192se/rtl8192se.ko $(MODDESTDIR)/rtl8192se
	@install -p -m 644 ./rtl8192c/rtl8192c-common.ko $(MODDESTDIR)/rtl8192c
	@install -p -m 644 ./rtl8192ce/rtl8192ce.ko $(MODDESTDIR)/rtl8192ce
	@install -p -m 644 ./rtl8192de/rtl8192de.ko $(MODDESTDIR)/rtl8192de
	@install -p -m 644 ./rtl8723ae/rtl8723ae.ko $(MODDESTDIR)/rtl8723ae
	@install -p -m 644 ./rtl8188ee/rtl8188ee.ko $(MODDESTDIR)/rtl8188ee
	@install -p -m 644 ./rtl8723be/rtl8723be.ko $(MODDESTDIR)/rtl8723be
	@install -p -m 644 ./rtl8192ee/rtl8192ee.ko $(MODDESTDIR)/rtl8192ee
	@install -p -m 644 ./rtl8821ae/rtl8821ae.ko $(MODDESTDIR)/rtl8821ae
	
	@depmod -a

	@#copy firmware img to target fold
	@#$(shell [ -d "$(FIRMWAREDIR)/`uname -r`" ] && cp -fr firmware/rtlwifi/ $(FIRMWAREDIR)/`uname -r`/.)
	@#$(shell [ ! -d "$(FIRMWAREDIR)/`uname -r`" ] && cp -fr firmware/rtlwifi/ $(FIRMWAREDIR)/.)
	@cp -fr firmware/rtlwifi/ $(FIRMWAREDIR)/
	@echo "Install rtlwifi SUCCESS"

uninstall:
	$(shell [ -d "$(MODDESTDIR)" ] && rm -fr $(MODDESTDIR))
	
	@depmod -a
	
	@#delete the firmware img
	@rm -fr /lib/firmware/rtlwifi/
	@rm -fr /lib/firmware/`uname -r`/rtlwifi/
	@echo "Uninstall rtlwifi SUCCESS"

clean:
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
	rm -fr Modules.symvers
	rm -fr Module.symvers
	rm -fr Module.markers
	rm -fr modules.order
	rm -fr tags
	@find -name "tags" -exec rm {} \;
	@rm -fr $(CLR_MODULE_FILES)
	@make -C btcoexist/ clean
	@make -C rtl8192c/ clean
	@make -C rtl8192ce/ clean
	@make -C rtl8192se/ clean
	@make -C rtl8192de/ clean
	@make -C rtl8723ae/ clean
	@make -C rtl8188ee/ clean
	@make -C rtl8723be/ clean
	@make -C rtl8192ee/ clean
	@make -C rtl8821ae/ clean
