#! /bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "killmodules.sh"
    echo
    echo "This script unloads all kernel modules required for dynamic clamp in relacs."
    echo "This includes comedi and rtai modules."
    echo
    echo "Call"
    echo "  loadmodules.sh --help"
    echo "for more information."
    echo
    exit 0
fi

function rm_module {
    lsmod | grep -q $1 && rmmod $1 && echo "removed $1"
}

# remove dynclamp modules:
rm_module dynclampmodule
rm_module dynclampaistreamingmodule

# remove all comedi modules:
lsmod | grep -q kcomedilib && modprobe -r kcomedilib && echo "removed kcomedilib"
for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
    modprobe -r $i && echo "removed $i"
done
lsmod | grep -q comedi && modprobe -r comedi && echo "removed comedi"

# remove rtai modules:
rm_module rtai_math
rm_module rtai_sched
rm_module rtai_hal
