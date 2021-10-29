#!/bin/bash

MAKE_COMEDI="${0##*/}"
VERSION_STRING="${MAKE_COMEDI} version 1.1 by Jan Benda, February 2021"

USE_DKMS=true


function print_version {
    echo $VERSION_STRING
}


function help_usage {
    cat <<EOF
$VERSION_STRING

Download, build and install comedi kernel modules and libraries.

usage:
sudo ${MAKE_COMEDI} ACTION

With ACTION one of
    packages       : install missing packages
    downloadkernel : download kernel headers and source packages
    prepare        : prepare kernel sources
    download       : download comedi
    remove         : remove comedi sources from dkms tree
    add            : add comedi sources to dkms tree
    build          : build comedi
    install        : install comedi
    clean          : clean comedi sources
    kill           : kill currently loaded comedi modules
    load           : load comedi modules
    setup          : setup comedi group and user permission
    comedilib      : download/update and build comedilib
    comedicalibrate: download/update and build comedi_calibrate

If no action is specified, then do everything necessary to build
the comedi kernel modules.
EOF
}


function check_root {
    if test "x$(id -u)" != "x0"; then
        echo "You need to be root to run this script!"
        echo "Try:"
        echo "  sudo $0"
        exit 1
    fi
}


function next_command {
    echo
    echo "==============================================================="
}


function install_packages {
    next_command
    echo "install all required packages ..."
    apt-get -y install build-essential dkms dpkg-dev gcc g++ git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev
}


function download_kernel {
    next_command
    echo "download kernel header ..."
    apt-get install linux-headers-`uname -r`

    if ! $USE_DKMS; then
	next_command
        echo "download kernel sources ..."
        cd /usr/src
	SRCPACKAGE=linux-source-`uname -r`
	echo "download $SRCPACKAGE ..."
	if ! apt-get source $SRCPACKAGE; then
	    MAJOR=$(uname -r | cut -d . -f1)
	    MINOR=$(uname -r | cut -d . -f2)
	    SRCPACKAGE=linux-source
	    for SOURCES in $(apt-cache search linux-source | awk '{print $1}'); do
		VERSION=${SOURCES:13}
		SRCMAJOR=$(echo $VERSION | cut -d . -f1)
		SRCMINOR=$(echo $VERSION | cut -d . -f2)
		if test x$MAJOR = x$SRCMAJOR -a x$MINOR = x$SRCMINOR; then
		    SRCPACKAGE=$SOURCES
		    break
		fi
	    done
	    echo "download $SRCPACKAGE ..."
	    if ! apt-get source $SRCPACKAGE; then
	        echo "FAILED TO INSTALL KERNEL SOURCE PACKAGE!"
		cd -
		exit 1
	    fi
	fi
	echo "unpack $SRCPACKAGE ..."
	tar xf $SRCPACKAGE.tar.bz2
	ln -sfn $(ls -rt | tail -n 1) linux
	cd -
    fi
}


function prepare_kernel {
    if ! $USE_DKMS; then
	next_command
        echo "prepare kernel ..."
	cd /usr/src/linux
	cp /boot/config-`uname -r` .config
	cp ../linux-headers-`uname -r`/Module.symvers .
	make silentoldconfig
	make prepare
	make scripts
	cd -
    fi
}


function download_comedi {
    next_command
    if test -d /usr/local/src/comedi; then
	echo "update comedi sources ..."
	cd /usr/local/src/comedi
	git pull origin master
	cd -
    else
	echo "download comedi sources ..."
	cd /usr/local/src
	git clone https://github.com/Linux-Comedi/comedi.git
	cd -
    fi
}


function remove_comedi {
    if $USE_DKMS; then
	next_command
	echo "remove comedi from dkms ..."
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms remove comedi/$VERSION --all
    fi
}


function add_comedi {
    if $USE_DKMS; then
	next_command
	echo "prepare comedi sources ..."
        cd /usr/local/src/comedi
	./autogen.sh
	cd -
	next_command
	echo "add comedi to dkms ..."
	dkms add /usr/local/src/comedi
    fi
}


function build_comedi {
    next_command
    if $USE_DKMS; then
	echo "build comedi via dkms ..."
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms build -m comedi -v $VERSION
    else
	echo "build comedi ..."
	cd /usr/local/src/comedi
	./autogen.sh
	./configure --with-linuxdir="/usr/src/linux"
	make -j$(grep -c "^processor" /proc/cpuinfo)
	cd -
    fi
}


function install_comedi {
    next_command
    echo "remove comedi from the kernel's staging directory ..."
    rm -r /lib/modules/`uname -r`/kernel/drivers/staging/comedi
    if $USE_DKMS; then
	echo "install comedi via dkms ..."
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms install -m comedi -v $VERSION
    else
	echo "install comedi ..."
	cd /usr/local/src/comedi
	make install
	depmod -a
	cp include/linux/comedi.h /usr/include/linux/
	cp include/linux/comedilib.h /usr/include/linux/
	cd -
    fi
}


function clean_comedi {
    next_command
    echo "clean comedi ..."
    cd /usr/local/src/comedi
    make clean
    cd -
}


function kill_comedi {
    next_command
    echo "unload all comedi modules ..."
    lsmod | grep -q kcomedilib || ( modprobe -r kcomedilib && echo "removed kcomedilib" )
    for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	modprobe -r $i && echo "removed $i"
    done
    lsmod | grep -q comedi || ( modprobe -r comedi && echo "removed comedi" )
}


function load_comedi {
    next_command
    echo "load comedi modules ..."
    udevadm trigger  # for comedi
    sleep 1
    test -c /dev/comedi0 && echo "loaded comedi" || echo "failed to load comedi modules"
}


function download_comedilib {
    next_command
    if test -d /usr/local/src/comedilib; then
	echo "update comedilib ..."
	cd /usr/local/src/comedilib
	git pull origin master
	cd -
    else
	echo "download comedilib ..."
	cd /usr/local/src
	git clone https://github.com/Linux-Comedi/comedilib.git
	cd -
    fi
}


function build_comedilib {
    next_command
    echo "build comedilib ..."
    cd /usr/local/src/comedilib
    ./autogen.sh
    ./configure --prefix=/usr --sysconfdir=/etc
    make -j$(grep -c "^processor" /proc/cpuinfo)
    make install
    cd -
}


function download_comedi_calibrate {
    next_command
    if test -d /usr/local/src/comedi_calibrate; then
	echo "update comedi_calibrate ..."
	cd /usr/local/src/comedi_calibrate
	git pull origin master
	cd -
    else
	echo "download comedi_calibrate ..."
	cd /usr/local/src
	git clone https://github.com/Linux-Comedi/comedi_calibrate.git
	cd -
    fi
}


function build_comedi_calibrate {
    next_command
    echo "build comedi_calibrate ..."
    cd /usr/local/src/comedi_calibrate
    autoreconf -v -i
    ./configure --prefix=/usr --sysconfdir=/etc
    make
    make install
    cd -
}


function setup_permissions {
    next_command
    groupadd --system iocard
    echo "setup udev permissions for comedi ..."
    echo 'KERNEL=="comedi*", MODE="0660", GROUP="iocard"' > /etc/udev/rules.d/95-comedi.rules
    udevadm trigger
    usermod $USER -a -G iocard   # or: adduser $USER iocard
}


###########################################################################
# main script:
while test "x${1:0:1}" = "x-"; do
    case $1 in
        --help )
            help_usage
            exit 0
            ;;

        --version )
            print_version
            exit 0
            ;;
    esac
    shift
done

check_root

if test -z "$1"; then
    install_packages
    download_kernel
    prepare_kernel
    download_comedi
    remove_comedi
    add_comedi
    kill_comedi
    build_comedi
    install_comedi
    next_command
    echo "Successfully built the comedi modules for the running kernel!"
    echo
    echo "You need to restart the computer to make the comedi modules available".
    echo
    exit 0
fi

case $1 in

    help ) 
        print_help
        exit 0
        ;;

    version ) 
        print_version
        exit 0
        ;;

    packages )
	install_packages
        exit 0
        ;;

    downloadkernel )
	download_kernel
        exit 0
        ;;

    prepare )
	prepare_kernel
        exit 0
        ;;

    download )
	download_comedi
        exit 0
        ;;

    remove )
	remove_comedi
        exit 0
        ;;

    add )
	add_comedi
        exit 0
        ;;

    build )
	build_comedi
        exit 0
        ;;

    install )
	install_comedi
        exit 0
        ;;

    clean )
	clean_comedi
        exit 0
        ;;

    kill )
	kill_comedi
        exit 0
        ;;

    load )
	load_comedi
        exit 0
        ;;

    setup )
	setup_permissions
        exit 0
        ;;

    comedilib )
	download_comedilib
	build_comedilib
        exit 0
        ;;

    comedicalibrate )
	download_comedi_calibrate
	build_comedi_calibrate
        exit 0
        ;;

esac
