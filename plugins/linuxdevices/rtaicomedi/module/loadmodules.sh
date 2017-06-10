#!/bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "loadmodules.sh"
    echo
    echo "This script loads all the necessary kernel modules that are needed"
    echo "for using relacs with dynamic clamp support."
    echo "This requires an RTAI patched linux kernel."
    echo "See ../doc/html/index.html or www.relacs.net/plugins/rtaicomedi/index.html for more information."
    echo
    echo "If you want to load the kernel modules automatically by the boot"
    echo "process of your linux system, simply call this script from /etc/rc.local"
    echo
    exit 0
fi

lsmod | grep -q rtai_hal || { insmod /usr/realtime/modules/rtai_hal.ko && echo "loaded rtai_hal"; }
lsmod | grep -q rtai_sched || { insmod /usr/realtime/modules/rtai_sched.ko && echo "loaded rtai_sched"; }
if test -f /usr/realtime/modules/rtai_math.ko; then
  lsmod | grep -q rtai_math || { insmod /usr/realtime/modules/rtai_math.ko && echo "loaded rtai_math"; }
else
  echo "rtai_math is not available"
fi
udevadm trigger  # for comedi
sleep 1
lsmod | grep -q kcomedilib || { modprobe kcomedilib && echo "loaded kcomedilib"; }

test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

MODULEPATH="${0%/*}"

lsmod | grep -q rtmodule && rmmod rtmodule && echo "removed rtmodule"
lsmod | grep -q dynclampmodule && rmmod dynclampmodule && echo "removed dynclampmodule"

#insmod $MODULEPATH/rtmodule.ko && echo "loaded $MODULEPATH/rtmodule.ko"
insmod $MODULEPATH/dynclampmodule.ko && echo "loaded $MODULEPATH/dynclampmodule.ko"
