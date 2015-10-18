rtlwifi_new
===========

A repo for the newest Realtek rtlwifi codes.

This code will build on any kernel 3.0 and newer. It includes the following drivers:

rtl8192ce, rtl8192se, rtl8192de, rtl8188ee, rtl8192ee, rtl8723ae, rtl8723be, and rtl8821ae.

####Installation instruction
How can you find <<YOUR WIRELESS DRIVER CODE>> using `lspci | grep Wireless`
```
sudo apt-get install linux-headers-generic build-essential git`
git clone https://github.com/lwfinger/rtlwifi_new.git
cd rtlwifi_new
make clean
sudo make install
sudo modprobe -r <<YOUR WIRELESS DRIVER CODE>>
sudo modprobe <<YOUR WIRELESS DRIVER CODE>>
sudo reboot
```

####Option Configuration
```
touch /etc/modprobe.d/<<YOUR WIRELESS DRIVER CODE>>.conf
vim /etc/modprobe.d/<<YOUR WIRELESS DRIVER CODE>>.conf 
```
and past bellow line
`options <<YOUR WIRELESS DRIVER CODE>> fwlps=N ips=N`
