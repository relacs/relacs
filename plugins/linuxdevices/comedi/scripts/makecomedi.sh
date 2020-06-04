#!/bin/bash

MAKE_COMEDI="${0##*/}"
VERSION_STRING="${MAKE_COMEDI} version 1.0 by Jan Benda, June 2020"

USE_DKMS=true


function print_version {
    echo $VERSION_STRING
}


function help_usage {
    cat <<EOF
$VERSION_STRING

Download, build and install comedi kernel modules.

usage:
sudo ${MAKE_COMEDI} ACTION

With ACTION one of
    packages       : install missing packages
    downloadkernel : download kernel headers and source packages
    prepare        : prepare kernel sources
    download       : download comedi
    add            : remove and add comedi sources to dkms tree
    build          : build comedi
    install        : install comedi
    clean          : clean comedi sources
    kill           : kill currently loaded comedi modules
    load           : load comedi modules
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


function install_packages {
    # install all required packages:
    apt-get -y install dpkg-dev gcc g++ git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev
}


function download_kernel {
    # install kernel headers:
    apt-get install linux-headers-`uname -r`

    # install kernel sources:
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
}


function prepare_kernel {
    # prepare kernel sources:
    cd /usr/src/linux
    cp /boot/config-`uname -r` .config
    cp ../linux-headers-`uname -r`/Module.symvers .
    make silentoldconfig
    make prepare
    make scripts
    cd -
}


function download_comedi {
    # download comedi sources:
    cd /usr/local/src
    git clone https://github.com/Linux-Comedi/comedi.git
    cd -
    if $USE_DKMS; then
	dkms add /usr/local/src/comedi
    fi
}


function add_comedi {
    # re-add comedi to dkms system:
    if $USE_DKMS; then
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms remove -m comedi -v $VERSION
	dkms add /usr/local/src/comedi
    fi
}


function build_comedi {
    if $USE_DKMS; then
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms build -j -m comedi -v $VERSION
    else
	cd /usr/local/src/comedi
	./autogen.sh
	./configure --with-linuxdir="/usr/src/linux"
	make -j$(grep -c "^processor" /proc/cpuinfo)
	cd -
    fi
}


function install_comedi {
    # remove comedi from the kernel's staging directory:
    rm -r /lib/modules/`uname -r`/kernel/drivers/staging/comedi
    if $USE_DKMS; then
	VERSION=$(grep PACKAGE_VERSION /usr/local/src/comedi/dkms.conf | cut -d = -f2)
	dkms install -m comedi -v $VERSION
    else
	# install comedi:
	cd /usr/local/src/comedi
	make install
	depmod -a
	cp include/linux/comedi.h /usr/include/linux/
	cp include/linux/comedilib.h /usr/include/linux/
	cd -
    fi
}


function clean_comedi {
    cd /usr/local/src/comedi
    make clean
    cd -
}


function kill_comedi {
    # remove all comedi modules:
    lsmod | grep -q kcomedilib || ( modprobe -r kcomedilib && echo "removed kcomedilib" )
    for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	modprobe -r $i && echo "removed $i"
    done
    lsmod | grep -q comedi || ( modprobe -r comedi && echo "removed comedi" )
}


function load_comedi {
    udevadm trigger  # for comedi
    sleep 1
    test -c /dev/comedi0 && echo "loaded comedi" || echo "failed to load comedi modules"
}


function download_comedilib {
    cd /usr/local/src
    git clone https://github.com/Linux-Comedi/comedilib.git
    cd -
}


function build_comedilib {
    cd /usr/local/src/comedilib
    ./autogen.sh
    ./configure --prefix=/usr --sysconfdir=/etc
    make -j$(grep -c "^processor" /proc/cpuinfo)
    make install
    cd -
}


function download_comedi_calibrate {
    cd /usr/local/src
    git clone https://github.com/Linux-Comedi/comedi_calibrate.git
    cd -
}


function build_comedi_calibrate {
    cd /usr/local/src/comedi_calibrate
    autoreconf -v -i
    ./configure --prefix=/usr --sysconfdir=/etc
    make
    make install
    cd -
}


function setup_permissions {
    # setup udev permissions:
    groupadd --system iocard
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
    build_comedi
    install_comedi
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

esac
