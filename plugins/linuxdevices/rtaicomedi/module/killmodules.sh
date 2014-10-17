#! /bin/bash
lsmod | grep -q dynclampmodule && rmmod dynclampmodule && echo "removed dynclampmodule"
lsmod | grep -q rtmodule && rmmod rtmodule && echo "removed rtmodule"

modprobe -r kcomedilib

lsmod | grep -q rtai_math && rmmod rtai_math
lsmod | grep -q rtai_fifos && rmmod rtai_fifos
lsmod | grep -q rtai_sched && rmmod rtai_sched
lsmod | grep -q rtai_hal && rmmod rtai_hal
