#! /bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "killmodules.sh"
    echo
    echo "This script unloads all kernel modules required for dynamic clamp in relacs."
    echo "This includes comedi and rtai module."
    echo
    echo "Call"
    echo "  loadmodules.sh --help"
    echo "for more information."
    echo
    exit 0
fi

# remove dynclamp modules:
lsmod | grep -q dynclampmodule && rmmod dynclampmodule && echo "removed dynclampmodule"
lsmod | grep -q rtmodule && rmmod rtmodule && echo "removed rtmodule"

# remove all comedi modules:
lsmod | grep -q kcomedilib || ( modprobe -r kcomedilib && echo "removed kcomedilib" )
for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
    modprobe -r $i && echo "removed $i"
done
lsmod | grep -q comedi || ( modprobe -r comedi && echo "removed comedi" )

# remove rtai modules:
lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed rtai_math"; }
lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed rtai_sched"; }
lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed rtai_hal"; }
