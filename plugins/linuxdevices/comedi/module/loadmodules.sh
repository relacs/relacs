#!/bin/bash
echo "please wait..."
insmod /usr/realtime/modules/rtai_hal.ko
insmod /usr/realtime/modules/rtai_ksched.ko
insmod /usr/realtime/modules/rtai_sem.ko
insmod /usr/realtime/modules/rtai_fifos.ko
modprobe kcomedilib
modprobe ni_pcimio
#modprobe ni_mio_cs
sleep 1
#comedi_config /dev/comedi0 ni_pcimio
#comedi_config /dev/comedi0 ni_mio_cs
#sleep 1 
#comedi_config /dev/comedi1 ni_pcimio
#sleep 4
#comedi_calibrate /dev/comedi0
#sleep 1 
#comedi_calibrate /dev/comedi1
#sleep 1
#for C in 0 $(seq 16); do for A in 0 1 2; do for R in 0 $(seq 20); do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 0 -r $R -a $A -c $C; done; done; done
#for C in 0 1; do for A in 0 1 2; do for R in 0 1; do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 1 -r $R -a $A -c $C; done; done; done

mknod -m 666 /dev/dynclamp c 227 0

insmod rtmodule.ko
#insmod dynclampmodule.ko
