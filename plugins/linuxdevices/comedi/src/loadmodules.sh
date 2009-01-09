#!/bin/bash
echo "please wait..."
modprobe ni_pcimio
comedi_config -r /dev/comedi16
comedi_config --read-buffer 640 --write-buffer 640 /dev/comedi16 ni_pcimio
comedi_soft_calibrate -f /dev/comedi16

#modprobe ni_mio_cs
#sleep 1
#comedi_config /dev/comedi0 ni_pcimio
#comedi_config /dev/comedi0 ni_mio_cs
#sleep 1 
#comedi_config /dev/comedi1 ni_pcimio
#sleep 1
#for C in 0 $(seq 16); do for A in 0 1 2; do for R in 0 $(seq 20); do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 0 -r $R -a $A -c $C; done; done; done
#for C in 0 1; do for A in 0 1 2; do for R in 0 1; do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 1 -r $R -a $A -c $C; done; done; done
#chmod a+rw /dev/comedi0
