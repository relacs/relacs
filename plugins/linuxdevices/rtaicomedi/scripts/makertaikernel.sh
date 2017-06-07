#!/bin/bash

###########################################################################
# you should modify the following parameter according to your needs:

KERNEL_PATH=/data/src         # where to put and compile the kernel
LINUX_KERNEL="4.4.43"         # linux vanilla kernel version (set with -k)
KERNEL_SOURCE_NAME="rtai"     # name for kernel source directory to be appended to LINUX_KERNEL
KERNEL_NAME="rtai"            # name for name of kernel to be appended to LINUX_KERNEL 
                              # (set with -n)

RTAI_DIR="rtai-5.0.1"         # name of the rtai source directory (set with -r):
                              # official relases for download (www.rtai.org):
                              # - rtai-4.1: rtai release version 4.1
                              # - rtai-5.0.1: rtai release version 5.0.1
                              # - rtai-x.x: rtai release version x.x
                              # from cvs (http://cvs.gna.org/cvsweb/?cvsroot=rtai):
                              # - magma: current development version
                              # - vulcano: stable development version
                              # Shahbaz Youssefi's RTAI clone on github:
                              # - RTAI: clone https://github.com/ShabbyX/RTAI.git
RTAI_PATCH="hal-linux-4.4.43-x86-6.patch" # rtai patch to be used
LOCAL_SRC_PATH=/usr/local/src # directory for downloading and building rtai, newlib and comedi
SHOWROOM_DIR=showroom         # target directory for rtai-showrom in ${LOCAL_SRC_PATH}

KERNEL_CONFIG="old" # whether and how to initialize the kernel configuration 
                    # (set with -c) 
                    # "old" for oldconfig from the running kernel,
                    # "def" for the defconfig target,
                    # "mod" for the localmodconfig target, (even if kernel do not match)
                    # or a full path to a config file.
                    # afterwards, the localmodconfig target is executed, 
                    # if the running kernel matches LINUX_KERNEL
RUN_LOCALMOD=true   # run make localmodconf after selecting a kernel configuration
KERNEL_DEBUG=false  # generate debuggable kernel (see man crash), set with -D


###########################################################################
# some global variables:

FULL_COMMAND_LINE="$@"
MAKE_RTAI_KERNEL="${0##*/}"

VERSION_STRING="${MAKE_RTAI_KERNEL} version 1.7 by Jan Benda, June 2017"
DRYRUN=false        # set with -d
RECONFIGURE_KERNEL=false
NEW_KERNEL_CONFIG=false
DEFAULT_RTAI_DIR="$RTAI_DIR"
RTAI_DIR_CHANGED=false
RTAI_PATCH_CHANGED=false
LINUX_KERNEL_CHANGED=false
RTAI_MENU=false     # enter RTAI configuration menu (set with -m)

NEW_KERNEL=false
NEW_RTAI=false
NEW_NEWLIB=false
NEW_COMEDI=false

CURRENT_KERNEL=$(uname -r)

MACHINE=$(uname -m)
RTAI_MACHINE=$MACHINE
if test "x$RTAI_MACHINE" = "xx86_64"; then
    RTAI_MACHINE="x86"
elif test "x$RTAI_MACHINE" = "xi686"; then
    RTAI_MACHINE="x86"
fi

CPU_NUM=$(grep -c "^processor" /proc/cpuinfo)


###########################################################################
# general functions:

function print_version {
    echo $VERSION_STRING
}

function print_usage {
    cat <<EOF
Download and build everything needed for an rtai-patched linux kernel with comedi and math support.

usage:
sudo ${MAKE_RTAI_KERNEL} [-d] [-n xxx] [-r xxx] [-p xxx] [-k xxx] [-c xxx] [-D] [-m] [action [target1 [target2 ... ]]]

-d    : dry run - only print out what the script would do, but do not execute any command
-n xxx: use xxx as the name of the new linux kernel (default ${KERNEL_NAME})
-k xxx: use linux kernel version xxx (default ${LINUX_KERNEL})
-r xxx: the rtai source (default ${RTAI_DIR}), one of
        magma: current rtai development version from csv
        vulcano: stable rtai development version from csv
        rtai-5.0.1: rtai release version 5.0.1 from www.rtai.org
        rtai-4.1: rtai release version 4.1 from www.rtai.org, or any other
        rtai-x.x: rtai release version x.x from www.rtai.org
        RTAI: snapshot from Shahbaz Youssefi's RTAI clone on github
-p xxx: use rtai patch file xxx
-c xxx: generate a new kernel configuration:
        old: use the kernel configuration of the currently running kernel
        def: generate a kernel configuration using make defconfig
        mod: simplify existing kernel configuration using make localmodconfig
             even if kernel do not match
        path/to/config/file: provide a particular configuration file
        afterwards (except for mod), make localmodconfig is executed to 
        deselect compilation of unused modules, 
        but only if the runnig kernel matches the selected kernel version.
-D    : generate kernel package with debug symbols in addition
-m    : enter the RTAI configuration menu

action can be one of
  help       : display this help message
  info       : display properties of rtai patches, loaded kernel modules, kernel, machine,
               and grub menu (no target required)
  info rtai  : list all available patches and suggest the one fitting to the kernel
  packages   : install required packages
  download   : download missing sources of the specified targets
  update     : update sources of the specified targets (not for kernel target)
  patch      : clean, unpack, and patch the linux kernel with the rtai patch (no target required)
  build      : compile and install the specified targets
               and the depending ones if needed
  install    : install the specified targets
  clean      : clean the source trees of the specified targets
  uninstall  : uninstall the specified targets
  remove     : remove the complete source trees of the specified targets
  reconfigure: reconfigure the kernel and make a full build of all targets (without target)
  reboot     : reboot into rtai kernel immediately (without target)
  test       : test the current kernel and write reports to the current working directory
  test all   : test the current kernel/kthreads/user and write reports to the current working directory

If no action is specified, a full download and build is performed for all targets (except showroom).

targets can be one or more of:
  kernel  : rtai-patched linux kernel
  newlib  : newlib library
  rtai    : rtai modules
  showroom: rtai showroom examples
            (supports only download, build, clean, remove)
  comedi  : comedi data acquisition driver modules
If no target is specified, all targets are made (except showroom).

Start with selecting and downloading an rtai source (-r option or RTAI_DIR variable):
$ sudo ${MAKE_RTAI_KERNEL} download rtai

Check for available patches:
$ sudo ${MAKE_RTAI_KERNEL} info rtai

Select a Linux kernel and a RTAI patch and
set the LINUX_KERNEL and RTAI_PATCH variables accordingly.


Common use cases:

$ sudo ${MAKE_RTAI_KERNEL}
  download and build all targets. A new configuration for the kernel is generated

$ sudo ${MAKE_RTAI_KERNEL} reconfigure
  build all targets using the existing configuration of the kernel

$ sudo ${MAKE_RTAI_KERNEL} test
  test the currently running kernel

$ sudo ${MAKE_RTAI_KERNEL} uninstall
  uninstall all targets
EOF
}

function check_root {
    if test "x$(id -u)" != "x0"; then
	echo "You need to be root to run this script!"
	echo "Try:"
	echo "  sudo $0 ${FULL_COMMAND_LINE}"
	exit 1
    fi
}

function print_info {
    echo
    echo "loaded modules (lsmod):"
    lsmod
    echo
    echo "distribution (lsb_release -a):"
    lsb_release -a 2> /dev/null
    echo
    echo "running kernel (uname -r):"
    uname -r
    echo
    echo "kernel parameter (/proc/cmdline):"
    cat /proc/cmdline
    echo
    echo "Revisions:"
    echo "  rtai  : ${RTAI_DIR} from $(cat ${LOCAL_SRC_PATH}/${RTAI_DIR}/revision.txt)"
    echo "  newlib: cvs from $(cat ${LOCAL_SRC_PATH}/newlib/revision.txt)"
    echo "  comedi: git from $(cat ${LOCAL_SRC_PATH}/comedi/revision.txt)"
    echo
    echo "CPU (/proc/cpuinfo):"
    grep "model name" /proc/cpuinfo | head -n 1
    echo "$CPU_NUM cores"
    echo "machine (uname -m): $MACHINE"
    echo "$(free -h | grep Mem | awk '{print $2}') RAM"
    echo
    echo "grub menu entries:"
    sed -n -e "/menuentry '/{s/.*'\\(.*\\)'.*/\\1/;p}" /boot/grub/grub.cfg
    echo
    echo "settings for ${MAKE_RTAI_KERNEL}:"
    echo "KERNEL_PATH=$KERNEL_PATH"
    echo "LINUX_KERNEL=$LINUX_KERNEL"
    echo "KERNEL_SOURCE_NAME=$KERNEL_SOURCE_NAME"
    echo "KERNEL_NAME=$KERNEL_NAME"
    echo "RTAI_DIR=$RTAI_DIR"
    echo "RTAI_PATCH=$RTAI_PATCH"

}

function print_full_info {
    RTAI_PATCH=""
    check_kernel_patch
    if test "x$1" != "xrtai"; then
	print_info
    fi
}


###########################################################################
# packages:
function install_packages {
    # required packages:
    echo "install packages:"
    if $DRYRUN; then
	echo "apt-get -y install make gcc libncurses-dev zlib1g-dev kernel-package libssl-dev libpci-dev libsensors4-dev g++ cvs git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev"
    else
	apt-get -y install make gcc libncurses-dev zlib1g-dev kernel-package libssl-dev libpci-dev libsensors4-dev g++ cvs git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev
    fi
}

###########################################################################
# linux kernel:

function check_kernel_patch {
    if test -z "$RTAI_PATCH"; then
	cd ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/
	echo
	echo "Available patches for this machine ($RTAI_MACHINE):"
	ls -rt -1 *.patch 2> /dev/null
	echo
	LINUX_KERNEL_V=${LINUX_KERNEL%.*}
	echo "Available patches for the selected kernel's kernel version ($LINUX_KERNEL_V):"
	ls -rtl -1 *${LINUX_KERNEL_V}*.patch 2> /dev/null
	echo
	echo "Available patches for the selected kernel ($LINUX_KERNEL):"
	ls -rtl -1 *${LINUX_KERNEL}*.patch 2> /dev/null
	RTAI_PATCH="$(ls -rt *-${LINUX_KERNEL}-*.patch 2> /dev/null | tail -n 1)"
	if test -z "$RTAI_PATCH"; then
	    RTAI_PATCH="$(ls -rt *.patch 2> /dev/null | tail -n 1)"
	fi
	if ! expr match $RTAI_PATCH ".*$LINUX_KERNEL" > /dev/null; then
	    if test "x${RTAI_PATCH:0:10}" = "xhal-linux-"; then
		LINUX_KERNEL=${RTAI_PATCH#hal-linux-}
		LINUX_KERNEL=${LINUX_KERNEL%%-*}
	    else
		LINUX_KERNEL="???"
	    fi
	fi
	cd - &> /dev/null
	echo
	echo "Choose a patch and set the RTAI_PATCH variable at the top of the script"
	echo "and the LINUX_KERNEL variable with the corresponding kernel version."
	echo
	echo "Suggested values:"
	echo
	echo "RTAI_PATCH=\"${RTAI_PATCH}\""
	echo "LINUX_KERNEL=\"${LINUX_KERNEL}\""
	echo
	return 1
    elif ! test -f ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/$RTAI_PATCH; then
	echo
	echo "Error: rtai patch file $RTAI_PATCH does not exist."
	echo "Run again with -p \"\" to see list of available patches."
	return 2
    elif ! expr match $RTAI_PATCH ".*$LINUX_KERNEL" > /dev/null; then
	echo
	echo "Error: kernel version ${LINUX_KERNEL} does not match rtai patch ${RTAI_PATCH}."
	echo "Specify a matching kernel with the -k option or by setting the LINUX_KERNEL variable."
	echo
	if test "x${RTAI_PATCH:0:10}" = "xhal-linux-"; then
	    LINUX_KERNEL=${RTAI_PATCH#hal-linux-}
	    LINUX_KERNEL=${LINUX_KERNEL%%-*}
	    echo "Suggested value:"
	    echo
	    echo "LINUX_KERNEL=\"${LINUX_KERNEL}\""
	    echo
	fi
	return 2
    fi
    return 0
}

function download_kernel {
    if ! test -d "$KERNEL_PATH"; then
	echo "path to kernel sources $KERNEL_PATH does not exist!"
	exit 1
    fi
    cd $KERNEL_PATH
    if ! check_kernel_patch; then
	exit 1
    fi
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo "keep already downloaded linux kernel archive"
    elif test -n "$LINUX_KERNEL"; then
	echo "download linux kernel version $LINUX_KERNEL"
	if ! $DRYRUN; then
	    wget https://www.kernel.org/pub/linux/kernel/v${LINUX_KERNEL:0:1}.x/linux-$LINUX_KERNEL.tar.xz
	fi
    else
	echo
	echo "Available patches for this machine ($RTAI_MACHINE):"
	ls -rt ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/*.patch | while read LINE; do echo "  ${LINE#${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/}"; done
	echo
	echo "You need to specify a linux kernel version!"
	echo "Choose one from the above list of available rtai patches (most recent one is at the bottom)"
	echo "and pass it to this script via the -k option or set the LINUX_KERNEL variable directly."
	exit 0
    fi
}

function unpack_kernel {
    if ! test -d "$KERNEL_PATH"; then
	echo "path to kernel sources $KERNEL_PATH does not exist!"
	exit 1
    fi
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo "keep already existing linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} directory."
	echo "remove it with $ ${MAKE_RTAI_KERNEL} clean kernel"
    else
	if ! test -f linux-$LINUX_KERNEL.tar.xz; then
	    echo "archive linux-$LINUX_KERNEL.tar.xz not found."
	    echo "download it with $ ${MAKE_RTAI_KERNEL} download kernel."
	fi
	# unpack:
	echo "unpack kernel sources from archive"
	if ! $DRYRUN; then
	    tar xf linux-$LINUX_KERNEL.tar.xz
	    mv linux-$LINUX_KERNEL linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    cd linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    make mrproper
	fi
	NEW_KERNEL=true
    fi

    # standard softlink to kernel:
    cd /usr/src
    ln -sfn $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} linux
}

function patch_kernel {
    cd /usr/src/linux
    if $NEW_KERNEL; then
	if ! check_kernel_patch; then
	    exit 1
	fi
	echo "apply rtai patch $RTAI_PATCH to kernel sources"
	if ! $DRYRUN; then
	    patch -p1 < ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/$RTAI_PATCH
	fi
    fi
}

function install_kernel {
    cd "$KERNEL_PATH"
    KERNEL_PACKAGE=$(ls linux-image-${LINUX_KERNEL}*-${KERNEL_NAME}_*.deb | tail -n 1)
    KERNEL_DEBUG_PACKAGE=$(ls linux-image-${LINUX_KERNEL}*-${KERNEL_NAME}-dbg_*.deb | tail -n 1)
    if test -f "$KERNEL_PACKAGE"; then
	echo "install kernel from debian package $KERNEL_PACKAGE"
	if ! $DRYRUN; then
	    dpkg -i "$KERNEL_PACKAGE"
	    if $KERNEL_DEBUG; then
		dpkg -i "$KERNEL_DEBUG_PACKAGE"
	    fi
	    GRUBMENU="$(sed -n -e "/menuentry '/{s/.*'\\(.*\\)'.*/\\1/;p}" /boot/grub/grub.cfg | grep "${LINUX_KERNEL}.*-${KERNEL_NAME}" | head -n 1)"
	    grub-reboot "$GRUBMENU"
	fi
    else
	echo "no kernel to install"
	return 1
    fi
}

function reboot_kernel {
    echo "reboot into ${LINUX_KERNEL}*-${KERNEL_NAME} kernel"
    if ! $DRYRUN; then
	GRUBMENU="$(sed -n -e "/menuentry '/{s/.*'\\(.*\\)'.*/\\1/;p}" /boot/grub/grub.cfg | grep "${LINUX_KERNEL}.*-${KERNEL_NAME} " | head -n 1)"
	grub-reboot "$GRUBMENU"
	reboot
    fi
}

function build_kernel {
    cd /usr/src/linux
    if $NEW_KERNEL || $RECONFIGURE_KERNEL; then

	if ! $RECONFIGURE_KERNEL; then
	    # clean:
	    echo "clean kernel sources"
	    if ! $DRYRUN; then
		make-kpkg clean
	    fi
	fi

	if $NEW_KERNEL_CONFIG; then
	    # kernel configuration:
	    if test "x$KERNEL_CONFIG" = "xdef"; then
		echo "use default configuration"
		if ! $DRYRUN; then
		    make defconfig
		fi
	    elif test "x$KERNEL_CONFIG" = "xold"; then
		echo "use configuration from running kernel (/boot/config-${CURRENT_KERNEL})"
		if ! $DRYRUN; then
		    cp /boot/config-${CURRENT_KERNEL} .config
		    make olddefconfig
		fi
	    elif test "x$KERNEL_CONFIG" = "xmod"; then
		echo "run make localmodconfig"
		if test ${CURRENT_KERNEL:0:${#LINUX_KERNEL}} != $LINUX_KERNEL ; then
		    echo "warning: kernel versions do not match (selected kernel is $LINUX_KERNEL, running kernel is $CURRENT_KERNEL)!"
		    echo "run make localmodconfig anyways"
		fi
		if ! $DRYRUN; then
		    make localmodconfig
		fi
		RUN_LOCALMOD=false
	    elif test -f "$KERNEL_CONFIG"; then
		echo "use configuration from $KERNEL_CONFIG"
		if ! $DRYRUN; then
		    cp "$KERNEL_CONFIG" .config
		    make olddefconfig
		fi
	    else
		echo "Unknown kernel configuration $KERNEL_CONFIG."
		exit 0
	    fi
	    if $RUN_LOCALMOD; then
		if test ${CURRENT_KERNEL:0:${#LINUX_KERNEL}} = $LINUX_KERNEL ; then
		    echo "run make localmodconfig"
		    if ! $DRYRUN; then
			yes "" | make localmodconfig
		    fi
		else
		    echo "cannot run make localmodconfig, because kernel version does not match"
		fi
	    fi
	else
	    echo "keep already existing .configure file for linux-${LINUX_KERNEL}-${KERNEL_NAME}."
	fi

	# build the kernel:
	echo "build the kernel"
	if ! $DRYRUN; then
	    export CONCURRENCY_LEVEL=$CPU_NUM
	    if $KERNEL_DEBUG; then
		make-kpkg --initrd --append-to-version -${KERNEL_NAME} --revision 1.0 --config menuconfig kernel_image kernel_debug
	    else
		make-kpkg --initrd --append-to-version -${KERNEL_NAME} --revision 1.0 --config menuconfig kernel-image
	    fi
 	    if test "x$?" != "x0"; then
		echo
		echo "Error: failed to build the kernel!"
		echo "Scroll up to see why."
		exit 1
	    fi
	fi

	# install:
	install_kernel
    else
	echo "keep already compiled linux ${LINUX_KERNEL}-${KERNEL_NAME} kernel."
    fi
}

function clean_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo "remove kernel sources $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
	if ! $DRYRUN; then
	    rm -r linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	fi
    fi
}

function uninstall_kernel {
    # kernel:
    if test ${CURRENT_KERNEL} = ${LINUX_KERNEL}-${KERNEL_NAME} -o ${CURRENT_KERNEL} = ${LINUX_KERNEL}.0-${KERNEL_NAME}; then
	echo "Cannot uninstall a running kernel!"
	echo "First boot into a different kernel. E.g. by executing"
	echo "$ sudo ./${MAKE_RTAI_KERNEL} reboot"
	return 1
    fi
    echo "uninstall kernel ${LINUX_KERNEL}-${KERNEL_NAME}"
    if ! $DRYRUN; then
	apt-get -y remove linux-image-${LINUX_KERNEL}-${KERNEL_NAME}
	apt-get -y remove linux-image-${LINUX_KERNEL}.0-${KERNEL_NAME}
    fi
    cd /usr/src/linux
}

function remove_kernel {
    cd $KERNEL_PATH
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo "remove kernel package $KERNEL_PATH/linux-$LINUX_KERNEL.tar.xz"
	if ! $DRYRUN; then
	    rm linux-$LINUX_KERNEL.tar.xz
	fi
    fi
    KERNEL_PACKAGE=$(ls linux-image-${LINUX_KERNEL}-${KERNEL_NAME}*.deb | tail -n 1)
    if test -f "$KERNEL_PACKAGE"; then
	echo "remove kernel package $KERNEL_PACKAGE"
	if ! $DRYRUN; then
	    rm "$KERNEL_PACKAGE"
	fi
    fi
}

function run_test {
    DIR=$1
    TEST=$2
    echo
    echo "running $DIR/$TEST test"
    TEST_RESULTS=results-$DIR-$TEST.dat
    TEST_DIR=/usr/realtime/testsuite/$DIR/$TEST
    rm -f $TEST_RESULTS
    cd $TEST_DIR
    rm -f $TEST_RESULTS
    trap true SIGINT   # ^C should terminate ./run but not this script
    ./run | tee results-$DIR-$TEST.dat
    trap - SIGINT
    cd -
    mv $TEST_DIR/$TEST_RESULTS .
}

function test_rtaikernel {
    if test ${CURRENT_KERNEL} != ${LINUX_KERNEL}-${KERNEL_NAME} -a ${CURRENT_KERNEL} != ${LINUX_KERNEL}.0-${KERNEL_NAME}; then
	echo "Need a running rtai kernel!"
	echo "First boot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel. E.g. by executing"
	echo "$ sudo ./${MAKE_RTAI_KERNEL} reboot"
	return 1
    fi

    if $DRYRUN; then
	echo "run some tests on currently running kernel ${LINUX_KERNEL}-${KERNEL_NAME}"
	return 0
    fi

    # report number:
    NUM=001
    LASTREPORT="$(ls latencies-${LINUX_KERNEL}-${RTAI_DIR}-*-* 2> /dev/null | tail -n 1)"
    if test -n "$LASTREPORT"; then
	LASTREPORT="${LASTREPORT#latencies-${LINUX_KERNEL}-${RTAI_DIR}-}"
	N="${LASTREPORT%-*-*}"
	N=$(expr $N + 1)
	NUM="$(printf "%03d" $N)"
    fi

    # remove latency file to force calibration:
    # this is for rtai5, for rtai4 the calibration tools needs to be run manually
    # see base/arch/x86/calibration/README
    if test -f /usr/realtime/calibration/latencies; then
	rm /usr/realtime/calibration/latencies
    fi

    # loading rtai kernel modules:
    RTAIMOD_FAILED=false
    RTAIMATH_FAILED=false
    lsmod | grep -q rtai_hal || { insmod /usr/realtime/modules/rtai_hal.ko && echo "loaded rtai_hal" || RTAMOD_FAILED=true; }
    lsmod | grep -q rtai_sched || { insmod /usr/realtime/modules/rtai_sched.ko && echo "loaded rtai_sched" || RTAMOD_FAILED=true; }
    lsmod | grep -q rtai_fifos || { insmod /usr/realtime/modules/rtai_fifos.ko && echo "loaded rtai_fifos" || RTAMOD_FAILED=true; }
    if test -f /usr/realtime/modules/rtai_math.ko; then
	lsmod | grep -q rtai_math || { insmod /usr/realtime/modules/rtai_math.ko && echo "loaded rtai_math" || RTAIMATH_FAILED=true; }
    else
	echo "rtai_math is not available"
    fi

    if ! $RTAIMOD_FAILED; then
	# loading comedi:
	echo -n "triggering comedi "
	udevadm trigger
	sleep 1
	echo -n "."
	sleep 1
	echo -n "."
	sleep 1
	echo "."
	modprobe kcomedilib && echo "loaded kcomedilib"

	# remove comedi modules:
	modprobe -r kcomedilib && echo "removed kcomedilib"
	for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	    modprobe -r $i && echo "removed $i"
	done
	modprobe -r comedi && echo "removed comedi"
    fi

    # remove rtai modules:
    lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed rtai_math"; }
    lsmod | grep -q rtai_fifos && { rmmod rtai_fifos && echo "removed rtai_fifos"; }
    lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed rtai_sched"; }
    lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed rtai_hal"; }

    if $RTAIMOD_FAILED; then
	REPORT="${LINUX_KERNEL}-${RTAI_DIR}-${NUM}-rtai-modules-failed"
	{
	    echo "failed to load rtai modules"
	    echo
	    echo "Revisions:"
	    echo "  kernel: ${LINUX_KERNEL}"
	    echo "  rtai  : ${RTAI_DIR} from $(cat ${LOCAL_SRC_PATH}/${RTAI_DIR}/revision.txt)"
	    echo "  newlib: cvs from $(cat ${LOCAL_SRC_PATH}/newlib/revision.txt)"
	    echo "  comedi: git from $(cat ${LOCAL_SRC_PATH}/comedi/revision.txt)"
	    echo
	    echo
	    echo "dmesg:"
	    echo
	    dmesg | tail -n 50
	} > latencies-$REPORT
	cp /boot/config-${LINUX_KERNEL}-${KERNEL_NAME} config-$REPORT
	chown --reference=. latencies-$REPORT
	chown --reference=. config-$REPORT
	echo
	echo "saved kernel configureation in: config-$REPORT"
	echo "saved test results in         : latencies-$REPORT"
	return
    fi
    echo "successfully loaded and unloaded rtai modules"

    # check for kernel log messages:
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
	return 1
    fi

    # kernel tests:
    run_test kern latency
    run_test kern switches
    run_test kern preempt
    if test "x$1" = "xall"; then
	run_test kthreads latency
	run_test kthreads switches
	run_test kthreads preempt
	run_test user latency
	run_test user switches
	run_test user preempt
    fi

    # report:
    echo "finished all tests"
    echo
    read -p 'Please enter a short name describing the configuration (empty: delete): ' NAME
    if test -n "$NAME"; then
	read -p 'Please enter a short description of the test result (empty: delete): ' RESULT
    fi
    if test -n "$NAME" -a -n "$RESULT"; then
	REPORT="${LINUX_KERNEL}-${RTAI_DIR}-${NUM}-${NAME}-${RESULT}"
	{
	    for TD in kern kthreads user; do
		for TN in latency switches preempt; do
		    TEST_RESULTS=results-$TD-$TN.dat
		    if test -f "$TEST_RESULTS"; then
			echo "$TD/$TN test:"
			sed -e '/^\*/d' $TEST_RESULTS
			rm $TEST_RESULTS
			echo
			echo
		    fi
		done
	    done
	    if $RTAIMATH_FAILED; then
		echo "Failed to load rtai_math module."
		echo
		echo
	    fi
	    print_info
	    echo
	    echo "rtai-info reports:"
	    /usr/realtime/bin/rtai-info
	    echo
	    echo "dmesg:"
	    echo
	    if test "x$1" = "xall"; then
		dmesg
	    else
		dmesg | tail -n 50
	    fi
	} > latencies-$REPORT
	cp /boot/config-${LINUX_KERNEL}-${KERNEL_NAME} config-$REPORT
	chown --reference=. latencies-$REPORT
	chown --reference=. config-$REPORT
	echo
	echo "saved kernel configuration to : config-$REPORT"
	echo "saved test results to         : latencies-$REPORT"
    fi
}


###########################################################################
# newlib:

function download_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib/src/newlib; then
	echo "keep already downloaded newlib sources"
    else
	echo "download newlib"
	if ! $DRYRUN; then
	    mkdir newlib
	    cd newlib
	    cvs -z 9 -d :pserver:anoncvs:anoncvs@sourceware.org:/cvs/src login  # password: anoncvs
	    cvs -z 9 -d :pserver:anoncvs@sourceware.org:/cvs/src co newlib
	    date +"%F %H:%M" > revision.txt
	    mkdir install
	fi
    fi
}

function update_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib/src/newlib; then
	echo "update already downloaded newlib sources"
	cd newlib
	cvs -d :pserver:anoncvs@sourceware.org:/cvs/src update
	date +"%F %H:%M" > revision.txt
	clean_newlib
    else
	download_newlib
    fi
}

function install_newlib {
    cd ${LOCAL_SRC_PATH}/newlib
    cd src/newlib
    echo "install newlib"
    if ! $DRYRUN; then
	make install
    fi
    NEW_NEWLIB=true
}

function build_newlib {
    cd ${LOCAL_SRC_PATH}/newlib
    if test -f install/$MACHINE/lib/libm.a; then
	echo "keep already installed newlib library"
    else
	cd src/newlib
	echo "build newlib"
	if ! $DRYRUN; then
	    NEWLIB_CFLAGS="-O2"
	    if test "$(grep CONFIG_64BIT /usr/src/linux/.config)" = 'CONFIG_64BIT=y'; then
		NEWLIB_CFLAGS="-O2 -mcmodel=kernel"
	    fi
	    ./configure --prefix=${LOCAL_SRC_PATH}/newlib/install --disable-shared --host="$MACHINE" CFLAGS="${NEWLIB_CFLAGS}"
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo "Failed to build newlib!"
		exit 1
	    fi
	    make install
	    if test "x$?" != "x0"; then
		echo "Failed to install newlib!"
		exit 1
	    fi
	fi
	NEW_NEWLIB=true
    fi
}

function clean_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo "clean newlib"
	if ! $DRYRUN; then
	    rm -r newlib/install/*
	    cd newlib/src/newlib
	    make clean
	fi
    fi
}

function uninstall_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo "uninstall newlib"
	if ! $DRYRUN; then
	    rm -r newlib/install/*
	fi
    fi
}

function remove_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo "remove ${LOCAL_SRC_PATH}/newlib"
	if ! $DRYRUN; then
	    rm -r newlib
	fi
    fi
}


###########################################################################
# rtai:

function download_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	echo "keep already downloaded rtai sources"
	cd $RTAI_DIR
	echo "run make distclean on rtai sources"
	if ! $DRYRUN; then
	    make distclean
	fi
	cd -
    else
	echo "download rtai sources $RTAI_DIR"
	if ! $DRYRUN; then
	    if test "x$RTAI_DIR" = "xmagma"; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $RTAI_DIR
	    elif test "x$RTAI_DIR" = "xvulcano"; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $RTAI_DIR
	    elif test "x$RTAI_DIR" = "xRTAI"; then
		git clone https://github.com/ShabbyX/RTAI.git
	    else
		wget https://www.rtai.org/userfiles/downloads/RTAI/${RTAI_DIR}.tar.bz2
		tar xf ${RTAI_DIR}.tar.bz2
	    fi
	    date +"%F %H:%M" > $RTAI_DIR/revision.txt
	fi
    fi
    echo "set soft link rtai -> $RTAI_DIR"
    if ! $DRYRUN; then
	ln -sfn $RTAI_DIR rtai
    fi
}

function update_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	cd $RTAI_DIR
	echo "run make distclean on rtai sources"
	if ! $DRYRUN; then
	    make distclean
	fi
	if test -d CVS; then
	    echo "update already downloaded rtai sources"
	    if ! $DRYRUN; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai update
		date +"%F %H:%M" > revision.txt
	    fi
	elif test -d .git; then
	    echo "update already downloaded rtai sources"
	    if ! $DRYRUN; then
		git pull
		date +"%F %H:%M" > revision.txt
	    fi
	fi
	cd -
	echo "set soft link rtai -> $RTAI_DIR"
	if ! $DRYRUN; then
	    ln -sfn $RTAI_DIR rtai
	fi
    else
	download_rtai
    fi
}

function build_rtai {
    cd ${LOCAL_SRC_PATH}/${RTAI_DIR}
    if $NEW_KERNEL || $NEW_NEWLIB || ! test -f base/sched/rtai_sched.ko; then
	echo "build rtai"
	if ! $DRYRUN; then
	    # path to newlib math library:
	    LIBM_PATH=$(find ${LOCAL_SRC_PATH}/newlib/install/ -name 'libm.a')
	    # number of CPUs:
	    CONFIG_NR_CPUS=$(grep CONFIG_NR_CPUS /usr/src/linux/.config)
	    RTAI_NUM_CPUS=${CONFIG_NR_CPUS#*=}
	    # check for configure script (no present in ShabbyX RTAI):
	    if ! test -x configure; then
		test -x autogen.sh && ./autogen.sh
	    fi
	    # start out with default configuration:
	    cp base/arch/${RTAI_MACHINE}/defconfig .rtai_config
	    # diff -u base/arch/${RTAI_MACHINE}/defconfig .rtai_config
	    # configure:
	    if grep 'CONFIG_RTAI_VERSION="5' .rtai_config; then
		# ./configure script options seem to be very outdated (new libmath support)! 
		# So, the following won't work:
		# ./configure --enable-cpus=${RTAI_NUM_CPUS} --enable-fpu --with-math-libm-dir=$LIBM_PATH
		patch <<EOF
--- base/arch/x86/defconfig	2016-04-06 10:01:34.000000000 +0200
+++ .rtai_config	2017-06-07 11:05:04.056936890 +0200
@@ -19,14 +19,15 @@
 CONFIG_RTAI_TESTSUITE=y
 CONFIG_RTAI_COMPAT=y
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
+CONFIG_RTAI_CPUS="$RTAI_NUM_CPUS"
 # CONFIG_RTAI_DIAG_TSC_SYNC is not set
 
 #
@@ -38,8 +39,11 @@
 #
 # CONFIG_RTAI_SCHED_ISR_LOCK is not set
 # CONFIG_RTAI_LONG_TIMED_LIST is not set
+CONFIG_RTAI_LATENCY_SELF_CALIBRATION_METRICS="1"
 CONFIG_RTAI_LATENCY_SELF_CALIBRATION_FREQ="10000"
 CONFIG_RTAI_LATENCY_SELF_CALIBRATION_CYCLES="10000"
+CONFIG_RTAI_KERN_BUSY_ALIGN_RET_DELAY="0"
+CONFIG_RTAI_USER_BUSY_ALIGN_RET_DELAY="0"
 CONFIG_RTAI_SCHED_LXRT_NUMSLOTS="150"
 CONFIG_RTAI_MONITOR_EXECTIME=y
 CONFIG_RTAI_ALLOW_RR=y
@@ -69,7 +73,10 @@
 # Other features
 #
 CONFIG_RTAI_USE_NEWERR=y
-# CONFIG_RTAI_MATH is not set
+CONFIG_RTAI_MATH=y
+CONFIG_RTAI_MATH_LIBM_TO_USE="1"
+CONFIG_RTAI_MATH_LIBM_DIR="${LIBM_PATH%/*}"
+# CONFIG_RTAI_MATH_KCOMPLEX is not set
 CONFIG_RTAI_MALLOC=y
 # CONFIG_RTAI_USE_TLSF is not set
 CONFIG_RTAI_MALLOC_VMALLOC=y
EOF
	    else
		patch <<EOF
--- base/arch/x86/defconfig	2015-03-09 11:42:51.000000000 +0100
+++ .rtai_config	2015-09-09 10:44:17.662656156 +0200
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
+CONFIG_RTAI_CPUS="$RTAI_NUM_CPUS"
 # CONFIG_RTAI_DIAG_TSC_SYNC is not set
 
 #
@@ -38,8 +39,10 @@
 #
 # CONFIG_RTAI_SCHED_ISR_LOCK is not set
 # CONFIG_RTAI_LONG_TIMED_LIST is not set
-CONFIG_RTAI_SCHED_LATENCY_SELFCALIBRATE=y
+# CONFIG_RTAI_SCHED_LATENCY_SELFCALIBRATE is not set
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
+CONFIG_RTAI_MATH_LIBM_DIR="${LIBM_PATH%/*}"
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
	    fi
	    make -f makefile oldconfig
	    if $RTAI_MENU; then
		make menuconfig
	    fi
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo "Failed to build rtai modules!"
		exit 1
	    fi
	    make install
	    if test "x$?" != "x0"; then
		echo "Failed to install rtai modules!"
		exit 1
	    fi
	fi
	NEW_RTAI=true
    else
	echo "keep already built rtai modules"
    fi
}

function clean_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	echo "clean rtai"
	if ! $DRYRUN; then
	    cd $RTAI_DIR
	    make clean
	fi
    fi
}

function install_rtai {
    cd ${LOCAL_SRC_PATH}/${RTAI_DIR}
    echo "install rtai"
    if ! $DRYRUN; then
	make install
    fi
    NEW_RTAI=true
}

function uninstall_rtai {
    if test -d /usr/realtime; then
	echo "uninstall rtai"
	if ! $DRYRUN; then
	    rm -r /usr/realtime
	fi
    fi
}

function remove_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	echo "remove rtai in ${LOCAL_SRC_PATH}/$RTAI_DIR"
	if ! $DRYRUN; then
	    rm -r $RTAI_DIR
	fi
    fi
    if test -f $RTAI_DIR.tar.*; then
	echo "remove ${LOCAL_SRC_PATH}/$RTAI_DIR.tar.*"
	if ! $DRYRUN; then
	    rm $RTAI_DIR.tar.*
	fi
    fi
}


###########################################################################
# rtai showroom:

function download_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo "keep already downloaded rtai-showroom sources"
	cd $SHOWROOM_DIR/v3.x
	echo "run make clean on rtai-showroom sources"
	if ! $DRYRUN; then
	    PATH="$PATH:/usr/realtime/bin"
	    make clean
	fi
	cd -
    else
	echo "download rtai-showroom sources"
	if ! $DRYRUN; then
	    cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $SHOWROOM_DIR
	    date +"%F %H:%M" > $SHOWROOM_DIR/revision.txt
	fi
    fi
}

function update_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo "update already downloaded rtai-showroom sources"
	cd $SHOWROOM_DIR
	cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai update
	date +"%F %H:%M" > revision.txt
	clean_showroom
    else
	download_showroom
    fi
}

function build_showroom {
    cd ${LOCAL_SRC_PATH}/$SHOWROOM_DIR/v3.x
    echo "build rtai showroom"
    if ! $DRYRUN; then
	PATH="$PATH:/usr/realtime/bin"
	make
	if test "x$?" != "x0"; then
	    echo "Failed to build rtai showroom!"
	    exit 1
	fi
    fi
}

function clean_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo "clean rtai showroom"
	if ! $DRYRUN; then
	    cd $SHOWROOM_DIR/v3.x
	    PATH="$PATH:/usr/realtime/bin"
	    make clean
	fi
    fi
}

function remove_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo "remove rtai showroom in ${LOCAL_SRC_PATH}/$SHOWROOM_DIR"
	if ! $DRYRUN; then
	    rm -r $SHOWROOM_DIR
	fi
    fi
}


###########################################################################
# comedi:

function download_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo "keep already downloaded comedi sources"
    else
	echo "download comedi"
	if ! $DRYRUN; then
	    git clone https://github.com/Linux-Comedi/comedi.git
	    date +"%F %H:%M" > comedi/revision.txt
	fi
    fi
}

function update_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo "update already downloaded comedi sources"
	cd comedi
	git pull
	date +"%F %H:%M" > revision.txt
	clean_comedi
    else
	download_comedi
    fi
}

function install_comedi {
    cd ${LOCAL_SRC_PATH}/comedi
    echo "install comedi"
    if ! $DRYRUN; then
	make install
	depmod -a
	sleep 1
	KERNEL_MODULES=/lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}
	if ! test -d "$KERNEL_MODULES"; then
	    KERNEL_MODULES=/lib/modules/${LINUX_KERNEL}.0-${KERNEL_NAME}
	fi
	cp ${LOCAL_SRC_PATH}/comedi/comedi/Module.symvers ${KERNEL_MODULES}/comedi/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedi.h /usr/include/linux/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedilib.h /usr/include/linux/
	udevadm trigger
    fi
    NEW_COMEDI=true
}

function build_comedi {
    if $NEW_RTAI || ! test -f ${LOCAL_SRC_PATH}/comedi/comedi/comedi.o; then
	cd ${LOCAL_SRC_PATH}/comedi
	echo "build comedi"
	if ! $DRYRUN; then
	    ./autogen.sh
	    PATH="$PATH:/usr/realtime/bin"
	    ./configure --with-linuxdir=/usr/src/linux --with-rtaidir=/usr/realtime
	    if $NEW_RTAI; then
		make clean
	    fi
	    cp /usr/realtime/modules/Module.symvers comedi/
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo "Failed to build comedi!"
		exit 1
	    fi
	    make install
	    if test "x$?" != "x0"; then
		echo "Failed to install comedi!"
		exit 1
	    fi
	    depmod -a
	    cp ${LOCAL_SRC_PATH}/comedi/comedi/Module.symvers /lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}/comedi/
	    cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedi.h /usr/include/linux/
	    cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedilib.h /usr/include/linux/
	    udevadm trigger
	fi
	NEW_COMEDI=true
    else
	echo "keep already installed comedi modules"
    fi
}

function clean_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo "clean comedi"
	cd comedi
	if ! $DRYRUN; then
	    make clean
	fi
    fi
}

function uninstall_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo "uninstall comedi"
	cd comedi
	if ! $DRYRUN; then
	    make uninstall
	fi
    fi
}

function remove_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo "remove ${LOCAL_SRC_PATH}/comedi"
	if ! $DRYRUN; then
	    rm -r comedi
	fi
    fi
}


###########################################################################
# actions:

function full_install {
    NEW_KERNEL_CONFIG=true
    check_root

    install_packages

    uninstall_kernel

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
    echo
}

function reconfigure {
    RECONFIGURE_KERNEL=true
    check_root

    uninstall_kernel
    uninstall_newlib
    uninstall_rtai
    uninstall_comedi

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
		showroom ) download_showroom ;;
		comedi ) download_comedi ;;
	    esac
	done
    fi
}

function update_all {
    check_root
    if test -z $1; then
	update_newlib
	update_rtai
	update_comedi
    else
	for TARGET; do
	    case $TARGET in
		newlib ) update_newlib ;;
		rtai ) update_rtai ;;
		showroom ) update_showroom ;;
		comedi ) update_comedi ;;
	    esac
	done
    fi
}

function build_all {
    check_root
    if test -z "$1"; then
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
		showroom ) build_showroom ;;
		comedi ) build_comedi ;;
	    esac
	done
    fi
}

function install_all {
    check_root
    if test -z "$1"; then
	install_kernel
	install_newlib
	install_rtai
	install_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) install_kernel ;;
		newlib ) install_newlib ;;
		rtai ) install_rtai ;;
		comedi ) install_comedi ;;
	    esac
	done
    fi
}

function clean_all {
    check_root
    if test -z "$1"; then
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
		showroom ) clean_showroom ;;
		comedi ) clean_comedi ;;
	    esac
	done
    fi
}

function uninstall_all {
    check_root
    if test -z "$1"; then
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
    if test -z "$1"; then
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
		showroom ) remove_showroom ;;
		comedi ) remove_comedi ;;
	    esac
	done
    fi
}


function clean_unpack_patch_kernel {
   check_root
   clean_kernel
   unpack_kernel
   patch_kernel
 }

###########################################################################
###########################################################################
# main script:

if test "x$1" = "x-d"; then
    shift
    DRYRUN=true
fi
if test "x$1" = "x-n"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	KERNEL_NAME=$1
	shift
    else
	echo "you need to specify a name for the kernel after the -n option"
	exit 1
    fi
fi
if test "x$1" = "x-r"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	RTAI_DIR=$1
	shift
	if test "xRTAI_DIR" != "x$DEFAULT_RTAI_DIR"; then
	    RTAI_DIR_CHANGED=true
	fi
    else
	echo "you need to specify an rtai distribution after the -r option"
	exit 1
    fi
fi
if test "x$1" = "x-p"; then
    shift
    if test "x$1" != "xreconfigure"; then
	RTAI_PATCH=$1
	shift
	RTAI_PATCH_CHANGED=true
    else
	echo "you need to specify an rtai patch file after the -p option"
	exit 1
    fi
fi
if test "x$1" = "x-k"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	LINUX_KERNEL=$1
	shift
	LINUX_KERNEL_CHANGED=true
    else
	echo "you need to specify a linux kernel version after the -k option"
	exit 1
    fi
fi
if test "x$1" = "x-c"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	KERNEL_CONFIG="$1"
	if test "x$KERNEL_CONFIG" != "xdef" -a "x$KERNEL_CONFIG" != "xold" -a "x$KERNEL_CONFIG" != "xmod" -a "x${KERNEL_CONFIG:0:1}" != "x/"; then
	    KERNEL_CONFIG="$PWD/$KERNEL_CONFIG"
	fi
	NEW_KERNEL_CONFIG=true
	shift
    else
	echo "you need to specify a kernel configuration after the -c option"
	exit 1
    fi
fi
if test "x$1" = "x-D"; then
    shift
    KERNEL_DEBUG=true
fi
if test "x$1" = "x-m"; then
    shift
    RTAI_MENU=true
fi

if $RTAI_DIR_CHANGED && ! $RTAI_PATCH_CHANGED && ! $LINUX_KERNEL_CHANGED; then
    echo "Warning: you changed rtai sources and you might need to adapt the linux kernel version and rtai patch file to it."
    sleep 2
fi

ACTION=$1
shift
case $ACTION in

    help ) print_usage ;;
    --help ) print_usage ;;

    version ) print_version ;;
    --version ) print_version ;;

    info ) print_full_info $@ ;;

    reconfigure ) reconfigure ;;

    test ) test_rtaikernel $@ ;;

    packages ) install_packages ;;
    download ) download_all $@ ;;
    update ) update_all $@ ;;
    patch ) clean_unpack_patch_kernel ;;
    build ) build_all $@ ;;
    install ) install_all $@ ;;
    clean ) clean_all $@ ;;
    uninstall ) uninstall_all $@ ;;
    remove ) remove_all $@ ;;
    reboot ) reboot_kernel ;;

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
