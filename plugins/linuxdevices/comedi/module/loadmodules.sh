#!/bin/bash
echo "please wait..."
insmod /usr/realtime/modules/rtai_sem.ko
insmod /usr/realtime/modules/rtai_fifos.ko
modprobe kcomedilib

test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

insmod rtmodule.ko

#insmod dynclampmodule.ko
