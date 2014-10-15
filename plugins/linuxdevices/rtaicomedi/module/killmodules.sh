#! /bin/bash
rmmod dynclampmodule
rmmod rtmodule
modprobe -r kcomedilib
rmmod rtai_math rtai_fifos rtai_sched rtai_hal
