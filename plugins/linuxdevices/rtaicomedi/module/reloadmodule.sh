#!/bin/bash

if test "x$1" = "x--help"; then
    echo
    echo "reloadmodule.sh [LATENCY]"
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


MODULE_PATH="${0%/*}"

FULLRELOAD=true

if $FULLRELOAD; then
    # completely remove rtai modules and reload them again:

    $MODULE_PATH/killmodules.sh
    $MODULE_PATH/loadmodules.sh $@

else
    # reload the dynclampmodule only:

    LOADED=false

    if lsmod | fgrep -q "rtmodule "; then
	rmmod rtmodule && echo "removed old rtmodule"
	insmod $MODULE_PATH/rtmodule.ko && echo "loaded $MODULE_PATH/rtmodule.ko"
	LOADED=true
    fi
    
    if lsmod | fgrep -q "dynclampmodule "; then
	rmmod dynclampmodule && echo "removed old dynclampmodule"
	insmod $MODULE_PATH/dynclampmodule.ko && echo "loaded $MODULE_PATH/dynclampmodule.ko"
	LOADED=true
    fi
    
    if ! $LOADED; then
	$MODULE_PATH/loadmodules.sh
    fi

fi
