#!/bin/bash

# This script reloads the dynclampmodule and/or rtmodule.
# You need to execute this script whenever these modules have been recompiled.

MODULEPATH="${0%/*}"

LOADED=no

if lsmod | fgrep -q "rtmodule "; then
  rmmod rtmodule && echo "removed old rtmodule"
  insmod $MODULEPATH/rtmodule.ko && echo "loaded $MODULEPATH/rtmodule.ko"
  LOADED=true
fi

if lsmod | fgrep -q "dynclampmodule "; then
  rmmod dynclampmodule && echo "removed old dynclampmodule"
  insmod $MODULEPATH/dynclampmodule.ko && echo "loaded $MODULEPATH/dynclampmodule.ko"
  LOADED=true
fi

if test $LOADED = no; then
  $MODULEPATH/loadmodules.sh
fi

