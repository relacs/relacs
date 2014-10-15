#!/bin/bash

# This script loads all the necessary kernal modules that are needed
# for using dynamic clamp.
# This requires an RTAI patched linux kernel.
# See www.relacs.net/plugins/rtaicomedi/index.html for more information.
# If you want to load the kernel modules automatically by the boot
# process of your linux system, simply call this script from /etc/rc.local

echo "please wait..."
insmod /usr/realtime/modules/rtai_hal.ko
insmod /usr/realtime/modules/rtai_sched.ko
insmod /usr/realtime/modules/rtai_fifos.ko
insmod /usr/realtime/modules/rtai_math.ko
udevadm trigger  # for comedi
modprobe kcomedilib

test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

MODULEPATH="${0%/*}"

#insmod $MODULEPATH/rtmodule.ko && echo "loaded $MODULEPATH/rtmodule.ko"

insmod $MODULEPATH/dynclampmodule.ko && echo "loaded $MODULEPATH/dynclampmodule.ko"
