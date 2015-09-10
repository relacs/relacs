#!/bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "reloadmodule.sh"
    echo
    echo "This script reloads the dynclampmodule and/or rtmodule."
    echo "You need to execute this script whenever these modules have been recompiled."
    echo
    echo "Call"
    echo "  loadmodules.sh --help"
    echo "for more information."
    echo
    exit 0
fi


MODULEPATH="${0%/*}"

FULLRELOAD=true

if $FULLRELOAD; then
    # completely remove rtai modules and reload them again:

    $MODULEPATH/killmodules.sh
    $MODULEPATH/loadmodules.sh

else
    # reload the dynclampmodule only:

    LOADED=false

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
    
    if $LOADED; then
	$MODULEPATH/loadmodules.sh
    fi

fi


