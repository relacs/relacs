#!/bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "loadmodules.sh [-c] [LATENCY]"
    echo
    echo "This script loads all the necessary kernel modules that are needed"
    echo "for using relacs with dynamic clamp support."
    echo "This requires an RTAI patched linux kernel."
    echo "See ../doc/html/index.html or www.relacs.net/plugins/rtaicomedi/index.html for more information."
    echo
    echo "-c   load only rtai_hal and rtai_sched needed for loading comedi modules"
    echo
    echo "The optional LATENCY parameter is a latency in ns that is passed"
    echo "as the kernel_latency and user_latency paameter to the rtai_sched module"
    echo "to avoid self calibration."
    echo
    echo "If you want to load the kernel modules automatically by the boot"
    echo "process of your linux system, simply call this script from /etc/rc.local"
    echo
    exit 0
fi

# find RTAI installation path:
RTAI_MODULE_PATH=""
MP=$(find /usr/realtime -name "$(uname -r)*" | tail -n 1)
if test -d /usr/realtime/$(uname -r)/modules; then
    RTAI_MODULE_PATH=/usr/realtime/$(uname -r)/modules
elif test -d /usr/realtime/modules; then
    RTAI_MODULE_PATH=/usr/realtime/modules
else
    echo "did not find RTAI modules in /usr/realtime"
    exit 1
fi

echo "use RTAI modules from $RTAI_MODULE_PATH"

# load modules needed for comedi only:
USE_DC=true
if test "$1" = "-c"; then
    USE_DC=false
    shift
fi

# set parameter for rtai_sched from command line:
SCHEDPARAM=""
test -n "$1" && SCHEDPARAM="kernel_latency=$1 user_latency=$1"

# load rtai modules:
lsmod | grep -q rtai_hal || { insmod $RTAI_MODULE_PATH/rtai_hal.ko && echo "loaded rtai_hal"; }
lsmod | grep -q rtai_sched || { insmod $RTAI_MODULE_PATH/rtai_sched.ko $SCHEDPARAM && echo "loaded rtai_sched $SCHEDPARAM"; }
if $USE_DC; then
    if test -f $RTAI_MODULE_PATH/rtai_math.ko; then
	lsmod | grep -q rtai_math || { insmod $RTAI_MODULE_PATH/rtai_math.ko && echo "loaded rtai_math"; }
    else
	echo "rtai_math is not available"
    fi
fi

# comedi:
udevadm trigger  # for comedi
sleep 1
test -c /dev/comedi0 && echo "loaded comedi"
if $USE_DC; then
    lsmod | grep -q kcomedilib || { modprobe kcomedilib && echo "loaded kcomedilib"; }

    # dynamic clamp module:
    test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

    MODULE_PATH="${0%/*}"

    lsmod | grep -q dynclampmodule && rmmod dynclampmodule && echo "removed dynclampmodule"

    insmod $MODULE_PATH/dynclampmodule.ko && echo "loaded $MODULE_PATH/dynclampmodule.ko"
fi
