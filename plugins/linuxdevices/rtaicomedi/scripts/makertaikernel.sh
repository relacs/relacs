#!/bin/bash

LINUX_KERNEL="3.14.17"      # linux vanilla kernel version
KERNEL_PATH=/data/src       # where to put and compile the kernel
LINUX_KERNEL="3.14.17"      # linux vanilla kernel version
KERNEL_SOURCE_NAME="rtai"   # name for kernel source directory to be appended to LINUX_KERNEL
KERNEL_NAME="rtai"          # name for name of kernel to be appended to LINUX_KERNEL

RTAI_PATCH="x86/patches/hal-linux-3.14.17-x86-6x.patch" # rtai patch to be used
RTAI_DIR="magma"          # name of the rtai source directory:
                          # magma: current development version
                          # rtai-4.1: rtai release version 4.1
                          # RTAI: snapshot from Shahbaz Youssefi's RTAI clone on github

RECONFIGURE_KERNEL=no
NEW_KERNEL_CONFIG=yes

NEW_KERNEL=no
NEW_RTAI=no
NEW_NEWLIB=no
NEW_COMEDI=no


###########################################################################
# general functions:
function print_usage {
    echo "Download and build everything needed for an rtai-patched linux kernel with comedi and math support."
    echo ""
    echo "usage:"
    echo "sudo makertaikernel [-n xxx] [-r xxx] [action [target1 [target2 ... ]]]"
    echo ""
    echo "-n xxx: use xxx ase name of the new linux kernel (default rtai)"
    echo "-r xxx: the rtai source, one of"
    echo "        magma: current rtai development version (default)"
    echo "        rtai-4.1: rtai release version 4.1"
    echo "        RTAI: snapshot from Shahbaz Youssefi's RTAI clone on github"
    echo ""
    echo "action can be one of"
    echo "  help       : display this help message"
    echo "  packages   : install required packages"
    echo "  download   : download missing sources of the specified targets"
    echo "  build      : compile and install the specified targets and the depending ones if needed"
    echo "  clean      : clean the source trees the specified targets"
    echo "  uninstall  : uninstall the specified targets"
    echo "  remove     : remove the complete source trees of the specified targets"
    echo "  reconfigure: reconfigure the kernel and make a full build of all targets"
    echo "  test       : test the current kernel and write reports to the current working directory"
    echo "if no action is specified, a full download and build is performed for all targets."
    echo ""
    echo "targets can be one or more of:"
    echo "  kernel : rtai-patched linux kernel"
    echo "  newlib : newlib library"
    echo "  rtai   : rtai modules"
    echo "  comedi : comedi data acquisition driver modules"
    echo "if no target is specified, all targets are made."
    echo ""
    echo "Common use cases:"
    echo ""
    echo "sudo makertaikernel"
    echo "  download and build all targets. A new configuration for the kernel is generates"
    echo ""
    echo "sudo makertaikernel reconfigure"
    echo "  build all targets using the existing configuration of the kernel"
    echo ""
    echo "sudo makertaikernel test"
    echo "  test the currently running kernel"
    echo ""
    echo "sudo makertaikernel uninstall"
    echo "  uninstall all targets"
    echo ""
}

function check_root {
    if test "x$(id -u)" != "x0"; then
	echo "you need to be root to run this script"
	exit 1
    fi
}


###########################################################################
# packages:
function install_packages {
    # required packages:
    apt-get -y install make gcc libncurses-dev zlib1g-dev kernel-package g++ git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev
}

###########################################################################
# linux kernel:

function download_kernel {
    cd $KERNEL_PATH
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo "keep already downloaded linux kernel archive"
    else
	echo "download linux kernel version $LINUX_KERNEL"
	wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-$LINUX_KERNEL.tar.xz
    fi
}

function unpack_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo "keep already existing linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} directory."
    else
	# unpack:
	echo "unpack kernel sources from archive"
	tar xf linux-$LINUX_KERNEL.tar.xz
	mv linux-$LINUX_KERNEL linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	cd linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	make mrproper
	NEW_KERNEL=yes
    fi

    # standard softlink to kernel:
    cd /usr/src
    ln -sfn $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} linux
}

function patch_kernel {
    cd /usr/src/linux
    if test "$NEW_KERNEL" = "yes"; then
	echo "apply rtai patch to kernel sources"
	patch -p1 < /usr/local/src/rtai/base/arch/$RTAI_PATCH
    fi
}

function build_kernel {
    cd /usr/src/linux
    if test "$NEW_KERNEL" = "yes" -o "$RECONFIGURE_KERNEL" = "yes"; then

	if test "$RECONFIGURE_KERNEL" != "yes"; then
	    # clean:
	    make-kpkg clean
	fi

	if test "$NEW_KERNEL_CONFIG" = "yes"; then
	    # kernel configuration:
	    cp /boot/config-`uname -r` .config
	    make olddefconfig
	    yes "" | make localmodconfig
	else
	    echo "keep already existing .configure file for linux-${LINUX_KERNEL}-${KERNEL_NAME}."
	fi

	# build the kernel:
	export CONCURRENCY_LEVEL=$(grep -c "^processor" /proc/cpuinfo)
	make-kpkg --initrd --append-to-version -${KERNEL_NAME} --revision 1.0 --config menuconfig kernel-image

	# install:
	if test -f ../linux-image-${LINUX_KERNEL}-${KERNEL_NAME}_1.0_amd64.deb; then
	    dpkg -i ../linux-image-${LINUX_KERNEL}-${KERNEL_NAME}_1.0_amd64.deb
	    GRUBMENU="$(sed -n -e "/menuentry '/{s/.*'\\(.*\\)'.*/\\1/;p}" /boot/grub/grub.cfg | grep 3.14.17-rtai1 | head -n 1)"
	    grub-reboot "$GRUBMENU"
	else
	    echo "failed to build kernel"
	    exit 1
	fi
    else
	echo "keep already compiled linux ${LINUX_KERNEL}-${KERNEL_NAME} kernel."
    fi
}

function clean_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo "remove kernel sources $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
	rm -r linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
    fi
}

function uninstall_kernel {
    # kernel:
    if test $(uname -r) = ${LINUX_KERNEL}-${KERNEL_NAME}; then
	echo "Cannot uninstall a running kernel!"
	echo "First boot into a different kernel."
	exit 1
    fi
    echo "uninstall kernel ${LINUX_KERNEL}-${KERNEL_NAME}"
    apt-get -y remove linux-image-${LINUX_KERNEL}-${KERNEL_NAME}
    cd /usr/src/linux
    if test -f ../linux-image-${LINUX_KERNEL}-${KERNEL_NAME}_1.0_amd64.deb; then
	rm ../linux-image-${LINUX_KERNEL}-${KERNEL_NAME}_1.0_amd64.deb
    fi
}

function remove_kernel {
    cd $KERNEL_PATH
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo "remove kernel package $KERNEL_PATH/linux-$LINUX_KERNEL.tar.xz"
	rm linux-$LINUX_KERNEL.tar.xz
    fi
}

function test_kernel {
    if test $(uname -r) != ${LINUX_KERNEL}-${KERNEL_NAME}; then
	echo "Need a running rtai kernel!"
	echo "First boot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel."
	exit 1
    fi

    # loading rtai kernel modules:
    lsmod | grep -q rtai_hal || { insmod /usr/realtime/modules/rtai_hal.ko && echo "loaded rtai_hal"; }
    lsmod | grep -q rtai_sched || { insmod /usr/realtime/modules/rtai_sched.ko && echo "loaded rtai_sched"; }
    lsmod | grep -q rtai_fifos || { insmod /usr/realtime/modules/rtai_fifos.ko && echo "loaded rtai_fifos"; }
    if test -f /usr/realtime/modules/rtai_math.ko; then
	lsmod | grep -q rtai_math || { insmod /usr/realtime/modules/rtai_math.ko && echo "loaded rtai_math"; }
    else
	echo "rtai_math is not available"
    fi
    #udevadm trigger  # for comedi
    #lsmod | grep -q kcomedilib || { modprobe kcomedilib && echo "loaded kcomedilib"; }

    # remove rtai modules:
    lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed rtai_math"; }
    lsmod | grep -q rtai_fifos && { rmmod rtai_fifos && echo "removed rtai_fifos"; }
    lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed rtai_sched"; }
    lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed rtai_hal"; }

    # latency test:
    LATENCY_TEST_DIR=/usr/realtime/testsuite/kern/latency
    cd $LATENCY_TEST_DIR
    rm -f latencies.dat
    trap true SIGINT   # ^C should terminate ./run but not this script
    ./run | tee latencies.dat
    trap - SIGINT
    cd -

    # switch test:
    if ! test -f /var/log/messages; then
	echo "/var/log/messages does not exist!"
	echo "enable it by modifying the file /etc/rsyslog.d/50-default.conf :"
	echo "uncomment the lines:"
	echo
	echo "  .=info;*.=notice;*.=warn;\\"
	echo "  auth,authpriv.none;\\"
	echo "  cron,daemon.none;\\"
	echo "  mail,news.none          -/var/log/messages"
	echo
	echo "and run"
	echo "$ restart rsyslog"
	echo
	exit 1
    fi
    SWITCHES_TEST_DIR=/usr/realtime/testsuite/kern/switches
    cd $SWITCHES_TEST_DIR
    rm -f switches.dat
    trap true SIGINT   # ^C should terminate ./run but not this script
    ./run | tee switches.dat
    trap - SIGINT
    cd -

    # preempt test:
    PREEMPT_TEST_DIR=/usr/realtime/testsuite/kern/preempt
    cd $PREEMPT_TEST_DIR
    rm -f preempt.dat
    trap true SIGINT   # ^C should terminate ./run but not this script
    ./run | tee preempt.dat
    trap - SIGINT
    cd -

    # report:
    read -p 'Please enter a short name describing the configuration (empty: delete): ' NAME
    if test -n "$NAME"; then
	read -p 'Please enter a short description of the test result (empty: delete): ' RESULT
    fi
    if test -n "$NAME" -a -n "$RESULT"; then
	REPORT="${LINUX_KERNEL}-${KERNEL_NAME}-${NAME}-${RESULT}-$(date '+%F-%R')"
	{
	    echo "Latency test:"
	    sed -e '/^\*/d' $LATENCY_TEST_DIR/latencies.dat
	    echo
	    echo
	    echo "Switches test:"
	    sed -e '/^\*/d' $SWITCHES_TEST_DIR/switches.dat
	    echo
	    echo
	    echo "Preempt test:"
	    sed -e '/^\*/d' $PREEMPT_TEST_DIR/preempt.dat
	    echo
	    echo
	    echo rtai-info reports:
	    echo
	    /usr/realtime/bin/rtai-info
	    echo
	    echo
	    echo lsmod:
	    echo
	    lsmod
	    echo
	    echo
	    echo dmesg:
	    echo
	    dmesg | tail -n 40 | grep RTAI
	} > latencies-$REPORT
	cp /boot/config-${LINUX_KERNEL}-${KERNEL_NAME} config-$REPORT
	chown --reference=. latencies-$REPORT
	chown --reference=. config-$REPORT
    fi
    rm $LATENCY_TEST_DIR/latencies.dat
    rm $SWITCHES_TEST_DIR/switches.dat
    rm $PREEMPT_TEST_DIR/preempt.dat
}


###########################################################################
# newlib:

function download_newlib {
    cd /usr/local/src
    if test -d newlib; then
	echo "keep already downloaded newlib sources"
    else
	echo "download newlib"
	mkdir newlib
	cd newlib
	cvs -z 9 -d :pserver:anoncvs@sourceware.org:/cvs/src login  # password: anoncvs
	cvs -z 9 -d :pserver:anoncvs@sourceware.org:/cvs/src co newlib
	mkdir install
    fi
}

function build_newlib {
    cd /usr/local/src/newlib
    if test -f install/lib/libm.a; then
	echo "keep already installed newlib library"
    else
	cd src/newlib
	./configure --prefix=/usr/local/src/newlib/install --disable-shared CFLAGS="-O2 -mcmodel=kernel"
	make -j$(grep -c "^processor" /proc/cpuinfo)
	make install
	NEW_NEWLIB=yes
    fi
}

function clean_newlib {
    cd /usr/local/src
    if test -d newlib; then
	echo "clean newlib"
	rm -r newlib/install/*
	cd newlib/src/newlib
	make clean
    fi
}

function uninstall_newlib {
    cd /usr/local/src
    if test -d newlib; then
	echo "uninstall newlib"
	rm -r newlib/install/*
    fi
}

function remove_newlib {
    cd /usr/local/src
    if test -d newlib; then
	echo "remove /usr/local/src/newlib"
	rm -r newlib
    fi
}


###########################################################################
# rtai:

function download_rtai {
    cd /usr/local/src
    if test -d $RTAI_DIR; then
	echo "keep already downloaded rtai sources"
	cd $RTAI_DIR
	make distclean
	cd -
    else
	echo "download rtai"
	if test "x$RTAI_DIR" = "xmagma"; then
	    cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $RTAI_DIR
	elif test "x$RTAI_DIR" = "xRTAI"; then
	    git clone https://github.com/ShabbyX/RTAI.git
	else
	    wget https://www.rtai.org/userfiles/downloads/RTAI/${RTAI_DIR}.tar.bz2
	    tar xf ${RTAI_DIR}.tar.bz2
	fi
    fi
    ln -sfn $RTAI_DIR rtai
}

function build_rtai {
    cd /usr/local/src/rtai
    if test $NEW_KERNEL || test $NEW_NEWLIB || ! test -f base/sched/rtai_sched.ko; then
	cp base/arch/${RTAI_PATCH%/*/*}/defconfig .rtai_config
	patch <<EOF
--- .defconfig  	2015-04-07 23:36:27.879550619 +0200
+++ .rtai_config	2015-04-07 23:41:44.834414279 +0200
@@ -17,16 +17,17 @@
 # CONFIG_RTAI_DOC_LATEX_NONSTOP is not set
 # CONFIG_RTAI_DBX_DOC is not set
 CONFIG_RTAI_TESTSUITE=y
-CONFIG_RTAI_COMPAT=y
+# CONFIG_RTAI_COMPAT is not set
 # CONFIG_RTAI_EXTENDED is not set
-CONFIG_RTAI_LXRT_NO_INLINE=y
-# CONFIG_RTAI_LXRT_STATIC_INLINE is not set
+# CONFIG_RTAI_LXRT_NO_INLINE is not set
+CONFIG_RTAI_LXRT_STATIC_INLINE=y
+CONFIG_RTAI_FORTIFY_SOURCE=""
 
 #
 # Machine (x86)
 #
 CONFIG_RTAI_FPU_SUPPORT=y
-CONFIG_RTAI_CPUS="2"
+CONFIG_RTAI_CPUS="$(grep -c "^processor" /proc/cpuinfo)"
 # CONFIG_RTAI_DIAG_TSC_SYNC is not set
 
 #
@@ -40,6 +41,8 @@
 # CONFIG_RTAI_LONG_TIMED_LIST is not set
 CONFIG_RTAI_SCHED_LATENCY_SELFCALIBRATE=y
 CONFIG_RTAI_SCHED_LATENCY="0"
+CONFIG_RTAI_KERN_BUSY_ALIGN_RET_DELAY="0"
+CONFIG_RTAI_USER_BUSY_ALIGN_RET_DELAY="0"
 CONFIG_RTAI_SCHED_LXRT_NUMSLOTS="150"
 CONFIG_RTAI_MONITOR_EXECTIME=y
 CONFIG_RTAI_ALLOW_RR=y
@@ -69,7 +72,10 @@
 # Other features
 #
 # CONFIG_RTAI_USE_NEWERR is not set
-# CONFIG_RTAI_MATH is not set
+CONFIG_RTAI_MATH=y
+CONFIG_RTAI_MATH_LIBM_TO_USE="1"
+CONFIG_RTAI_MATH_LIBM_DIR="/usr/local/src/newlib/install/lib"
+# CONFIG_RTAI_MATH_KCOMPLEX is not set
 CONFIG_RTAI_MALLOC=y
 # CONFIG_RTAI_USE_TLSF is not set
 CONFIG_RTAI_MALLOC_VMALLOC=y
@@ -82,9 +88,7 @@
 #
 # Add-ons
 #
-CONFIG_RTAI_COMEDI_LXRT=y
-CONFIG_RTAI_COMEDI_DIR="/usr/comedi"
-# CONFIG_RTAI_USE_COMEDI_LOCK is not set
+# CONFIG_RTAI_COMEDI_LXRT is not set
 # CONFIG_RTAI_CPLUSPLUS is not set
 # CONFIG_RTAI_RTDM is not set
EOF
        make oldconfig
	make
	make install
	NEW_RTAI=yes
    else
	echo "keep already built rtai modules"
    fi
}

function clean_rtai {
    cd /usr/local/src
    if test -d $RTAI_DIR; then
	echo "clean rtai"
	cd $RTAI_DIR
	make clean
    fi
}

function uninstall_rtai {
    if test -d /usr/realtime; then
	echo "uninstall rtai"
	rm -r /usr/realtime
    fi
}

function remove_rtai {
    cd /usr/local/src
    if test -d $RTAI_DIR; then
	echo "remove rtai in /usr/local/src/$RTAI_DIR"
	rm -r $RTAI_DIR
    fi
}


###########################################################################
# comedi:

function download_comedi {
    cd /usr/local/src
    if test -d comedi; then
	echo "keep already downloaded comedi sources"
    else
	echo "download comedi"
	git clone git://comedi.org/git/comedi/comedi.git
    fi
}

function build_comedi {
    if test $NEW_RTAI = yes || ! test -f /usr/local/src/comedi/comedi/comedi.o; then
	cd /usr/local/src/comedi
	cp /usr/realtime/modules/Module.symvers comedi/
	./autogen.sh
	PATH="$PATH:/usr/realtime/bin"
	./configure --with-linuxdir=/usr/src/linux --with-rtaidir=/usr/realtime
	if test $NEW_RTAI= yes; then
	    make clean
	fi
	make -j$(grep -c "^processor" /proc/cpuinfo)
	make install
	depmod -a
	cp /usr/local/src/comedi/comedi/Module.symvers /lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}/comedi/
	cp /usr/local/src/comedi/include/linux/comedi.h /usr/include/linux/
	cp /usr/local/src/comedi/include/linux/comedilib.h /usr/include/linux/
	NEW_COMEDI=yes
    else
	echo "keep already installed comedi modules"
    fi
}

function clean_comedi {
    cd /usr/local/src
    if test -d comedi; then
	echo "clean comedi"
	cd comedi
	make clean
    fi
}

function uninstall_comedi {
    cd /usr/local/src
    if test -d comedi; then
	echo "uninstall comedi"
	cd comedi
	make uninstall
    fi
}

function remove_comedi {
    cd /usr/local/src
    if test -d comedi; then
	echo "remove /usr/local/src/comedi"
	rm -r comedi
    fi
}


###########################################################################
# actions:

function full_install {
    check_root

    install_packages

    download_rtai
    download_newlib
    download_comedi
    download_kernel

    unpack_kernel
    patch_kernel
    build_kernel

    build_newlib
    build_rtai
    build_comedi

    echo
    echo "Done!"
    echo "Please reboot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel"
}

function reconfigure {
    RECONFIGURE_KERNEL=yes
    NEW_KERNEL_CONFIG=no
    check_root

    unpack_kernel
    patch_kernel
    build_kernel

    build_newlib
    build_rtai
    build_comedi

    echo
    echo "Done!"
    echo "Please reboot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel"
}

function download_all {
    check_root
    if test -z $1; then
	download_kernel
	download_newlib
	download_rtai
	download_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) download_kernel ;;
		newlib ) download_newlib ;;
		rtai ) download_rtai ;;
		comedi ) download_comedi ;;
	    esac
	done
    fi
}

function build_all {
    check_root
    if test -z $1; then
	unpack_kernel
	patch_kernel
	build_kernel
	build_newlib
	build_rtai
	build_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) unpack_kernel
		    patch_kernel
		    build_kernel
		    build_newlib
		    build_rtai
		    build_comedi ;;
		newlib ) build_newlib
		    build_rtai
		    build_comedi ;;
		rtai ) build_rtai
		    build_comedi ;;
		comedi ) build_comedi ;;
	    esac
	done
    fi
}

function clean_all {
    check_root
    if test -z $1; then
	clean_kernel
	clean_newlib
	clean_rtai
	clean_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) clean_kernel ;;
		newlib ) clean_newlib ;;
		rtai ) clean_rtai ;;
		comedi ) clean_comedi ;;
	    esac
	done
    fi
}

function uninstall_all {
    check_root
    if test -z $1; then
	uninstall_kernel
	uninstall_newlib
	uninstall_rtai
	uninstall_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) uninstall_kernel ;;
		newlib ) uninstall_newlib ;;
		rtai ) uninstall_rtai ;;
		comedi ) uninstall_comedi ;;
	    esac
	done
    fi
}

function remove_all {
    check_root
    if test -z $1; then
	remove_kernel
	remove_newlib
	remove_rtai
	remove_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) remove_kernel ;;
		newlib ) remove_newlib ;;
		rtai ) remove_rtai ;;
		comedi ) remove_comedi ;;
	    esac
	done
    fi
}


###########################################################################
###########################################################################
# main script:

if test "x$1" = "x-n"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	KERNEL_NAME=$1
	shift
    else
	echo "you need to specify a name for the kernel after the -n option"
	exit 1
    fi
elif test "x$1" = "x-r"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	RTAI_DIR=$1
	shift
    else
	echo "you need to specify an rtai distribution after the -r option"
	exit 1
    fi
fi

ACTION=$1
shift
case $ACTION in

    help ) print_usage ;;

    reconfigure ) reconfigure ;;

    test ) test_kernel ;;

    packages ) intall_packages ;;
    download ) download_all $@ ;;
    build ) build_all $@ ;;
    clean ) clean_all $@ ;;
    uninstall ) uninstall_all $@ ;;
    remove ) remove_all $@ ;;

    * ) if test -n "$1"; then
	    echo "unknown option \"$1\""
	    echo
	    print_usage
	    exit 1
	else
	    full_install
	fi ;;

esac

exit 0
