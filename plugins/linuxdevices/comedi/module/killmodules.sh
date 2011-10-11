#! /bin/bash
rmmod dynclampmodule
rmmod rtmodule
sleep 0.1
modprobe -r kcomedilib
sleep 0.1
modprobe -r rtai_fifos
sleep 0.1
modprobe -r rtai_math
sleep 0.1
modprobe -r ni_pcimio
modprobe -r ni_mio_cs

