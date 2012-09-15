#!/bin/bash

# This script reloads the dynclampmodule and/or rtmodule.
# You need to execute this script whenever these modules have been recompiled.

MODULEPATH="${0%/*}"

if lsmod | fgrep -q "rtmodule "; then
  rmmod rtmodule && echo "removed old rtmodule"
  insmod $MODULEPATH/rtmodule.ko && echo "loaded $MODULEPATH/rtmodule.ko"
fi

if lsmod | fgrep -q "dynclampmodule "; then
  rmmod dynclampmodule && echo "removed old dynclampmodule"
  insmod $MODULEPATH/dynclampmodule.ko && echo "loaded $MODULEPATH/dynclampmodule.ko"
fi

