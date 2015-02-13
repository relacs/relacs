#!/bin/bash

# This script loads all the necessary kernal modules that are needed
# for using dynamic clamp.
# This requires an RTAI patched linux kernel.
# See ../doc/html/index.html or www.relacs.net/plugins/rtaicomedi/index.html for more information.
# If you want to load the kernel modules automatically by the boot
# process of your linux system, simply call this script from /etc/rc.local

lsmod | grep -q rtai_hal || { insmod /usr/realtime/modules/rtai_hal.ko && echo "loaded rtai_hal"; }
lsmod | grep -q rtai_sched || { insmod /usr/realtime/modules/rtai_sched.ko && echo "loaded rtai_sched"; }
lsmod | grep -q rtai_fifos || { insmod /usr/realtime/modules/rtai_fifos.ko && echo "loaded rtai_fifos"; }
if test -f /usr/realtime/modules/rtai_math.ko; then
  lsmod | grep -q rtai_math || { insmod /usr/realtime/modules/rtai_math.ko && echo "loaded rtai_math"; }
else
  echo "rtai_math is not available"
fi
udevadm trigger  # for comedi
lsmod | grep -q kcomedilib || { modprobe kcomedilib && echo "loaded kcomedilib"; }

test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

MODULEPATH="${0%/*}"

lsmod | grep -q rtmodule && rmmod rtmodule && echo "removed rtmodule"
lsmod | grep -q dynclampmodule && rmmod dynclampmodule && echo "removed dynclampmodule"

#insmod $MODULEPATH/rtmodule.ko && echo "loaded $MODULEPATH/rtmodule.ko"
insmod $MODULEPATH/dynclampmodule.ko && echo "loaded $MODULEPATH/dynclampmodule.ko"
