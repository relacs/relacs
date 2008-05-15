#!/bin/bash
echo "please wait..."
insmod /usr/src/kernels/rtai/modules/rtai_hal.ko
insmod /usr/src/kernels/rtai/modules/rtai_ksched.ko
insmod /usr/src/kernels/rtai/modules/rtai_sem.ko
insmod /usr/src/kernels/rtai/modules/rtai_math.ko
modprobe kcomedilib
modprobe ni_pcimio
sleep 5
comedi_config /dev/comedi0 ni_pcimio
sleep 1 
#comedi_config /dev/comedi1 ni_pcimio
sleep 4
comedi_calibrate /dev/comedi0
sleep 1 
#comedi_calibrate /dev/comedi1
mknod -m 666 /dev/dynclamp c 227 0

chmod a+rw /dev/comedi0
chmod a+rw /dev/comedi1
insmod dynclamp.ko
