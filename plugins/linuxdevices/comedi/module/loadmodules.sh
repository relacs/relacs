#!/bin/bash
echo "please wait..."
modprobe rtai_fifos
modprobe kcomedilib

test -c /dev/dynclamp || mknod -m 666 /dev/dynclamp c 227 0

#insmod rtmodule.ko && echo "loaded rtmodule.ko"

insmod dynclampmodule.ko && echo "loaded dynclampmodule.ko"
