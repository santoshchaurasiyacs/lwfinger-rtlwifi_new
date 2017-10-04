rtlwifi_new
===========

A repo for the newest Realtek rtlwifi codes.

This code will build on any kernel 4.2 and newer. It includes the following drivers:

rtl8192ce, rtl8192se, rtl8192de, rtl8188ee, rtl8192ee, rtl8723ae, rtl8723be, and rtl8821ae.

The reason for not building on kernels v4.1 and older is that my system has gcc7, but v4.1 is
missing the header file linux/compiler-gcc7.h. If anyone needs to build with older kernels,
you will need to apply any patches yourself. Of course, I will be happy to apply these patches
to the repository.

