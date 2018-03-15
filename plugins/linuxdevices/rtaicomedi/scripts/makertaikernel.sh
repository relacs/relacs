#!/bin/bash

###########################################################################
# you should modify the following parameter according to your needs:

: ${KERNEL_PATH:=/usr/src}       # where to put and compile the kernel (set with -s)
: ${LINUX_KERNEL:="4.4.115"}      # linux vanilla kernel version (set with -k)
: ${KERNEL_SOURCE_NAME:="rtai"}  # name for kernel source directory to be appended to LINUX_KERNEL
: ${KERNEL_NAME:="rtai1"}        # name for name of kernel to be appended to LINUX_KERNEL 
                                 # (set with -n)

: ${LOCAL_SRC_PATH:=/usr/local/src} # directory for downloading and building 
                              # rtai, newlib and comedi

: ${RTAI_DIR="rtai-5.1"}      # name of the rtai source directory (set with -r):
                              # official relases for download (www.rtai.org):
                              # - rtai-4.1: rtai release version 4.1
                              # - rtai-5.1: rtai release version 5.1
                              # - rtai-x.x: rtai release version x.x
                              # from cvs (http://cvs.gna.org/cvsweb/?cvsroot=rtai):
                              # - magma: current development version
                              # - vulcano: stable development version
                              # Shahbaz Youssefi's RTAI clone on github:
                              # - RTAI: clone https://github.com/ShabbyX/RTAI.git
: ${RTAI_PATCH:="hal-linux-4.4.115-x86-10.patch"} # rtai patch to be used
: ${SHOWROOM_DIR:=showroom}     # target directory for rtai-showrom in ${LOCAL_SRC_PATH}

: ${KERNEL_CONFIG:="old"}  # whether and how to initialize the kernel configuration 
                           # (set with -c) 
                           # "old" for oldconfig from the running kernel,
                           # "def" for the defconfig target,
                           # "mod" for the localmodconfig target, (even if kernel do not match)
                           # or a full path to a config file.
                           # afterwards, the localmodconfig target is executed, 
                           # if the running kernel matches LINUX_KERNEL
: ${RUN_LOCALMOD:=true}    # run make localmodconf after selecting a kernel configuration (disable with -l)
: ${KERNEL_DEBUG:=false}   # generate debuggable kernel (see man crash), set with -D

: ${NEWLIB_TAR:=newlib-3.0.0.20180226.tar.gz}  # tar file of current newlib version 
                                               # at ftp://sourceware.org/pub/newlib/index.html
                                               # in case git does not work

: ${MAKE_NEWLIB:=true}       # for automatic targets make newlib library
: ${MAKE_RTAI:=true}         # for automatic targets make rtai library
: ${MAKE_COMEDI:=true}       # for automatic targets make comedi library


###########################################################################
# some global variables:

FULL_COMMAND_LINE="$@"
MAKE_RTAI_KERNEL="${0##*/}"

VERSION_STRING="${MAKE_RTAI_KERNEL} version 1.8 by Jan Benda, June 2017"
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

LOG_FILE="${PWD}/${MAKE_RTAI_KERNEL%.*}.log"


###########################################################################
# general functions:

function echo_log {
    echo "$@" | tee -a "$LOG_FILE"
}

function print_version {
    echo $VERSION_STRING
}

function print_usage {
    cat <<EOF
Download and build everything needed for an rtai-patched linux kernel with comedi and math support.

usage:
sudo ${MAKE_RTAI_KERNEL} [-d] [-s xxx] [-n xxx] [-r xxx] [-p xxx] [-k xxx] [-c xxx] [-l] [-D] [-m] [action [target1 [target2 ... ]]]

-d    : dry run - only print out what the script would do, but do not execute any command
-s xxx: use xxx as the base directory where to put the kernel sources (default ${KERNEL_PATH})
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
        but only if the runnig kernel matches the selected kernel version (major.minor only).
-l    : disable call to make localmodconf after a kernel configuration 
        has been selected via the -c switch
-D    : generate kernel package with debug symbols in addition
-m    : enter the RTAI configuration menu

action can be one of
  help       : display this help message
  info       : display properties of rtai patches, loaded kernel modules, kernel, machine,
               and grub menu (no target required)
  info rtai  : list all available patches and suggest the one fitting to the kernel
  info grub  : show grub boot menu entries
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
  reboot X   : reboot into kernel specified by grub menu entry X
  test       : test the current kernel and write reports to the current working directory
  test all   : test the current kernel/kthreads/user and write reports to the current working directory
  test none   : test loading and unloading of rtai kernel modules and do not run any tests

If no action is specified, a full download and build is performed for all targets (except showroom).

targets can be one or more of:
  packages: required packages (install only)
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

function print_grub {
    echo "grub menu entries:"
    IFSORG="$IFS"
    IFS=$'\n'
    N=0
    for gm in $(grep '^menuentry' /boot/grub/grub.cfg | cut -d "'" -f 2); do
	echo "$N) $gm"
	let N+=1
    done
    IFS="$IFSORG"
}

function print_versions {
    echo "Versions:"
    echo "  kernel: ${LINUX_KERNEL}"
    echo "  gcc   : $(gcc --version | head -n 1)"
    if test -f ${LOCAL_SRC_PATH}/${RTAI_DIR}/revision.txt; then
	echo "  rtai  : ${RTAI_DIR} from $(cat ${LOCAL_SRC_PATH}/${RTAI_DIR}/revision.txt)"
	echo "  patch : ${RTAI_PATCH}"
    elif test -d ${LOCAL_SRC_PATH}/${RTAI_DIR}; then
	echo "  rtai  : ${RTAI_DIR} revision not available"
	echo "  patch : ${RTAI_PATCH}"
    else
	echo "  rtai  : not available"
    fi
    if test -f ${LOCAL_SRC_PATH}/newlib/src/revision.txt; then
	echo "  newlib: git from $(cat ${LOCAL_SRC_PATH}/newlib/src/revision.txt)"
    elif test -d ${LOCAL_SRC_PATH}/newlib; then
	echo "  newlib: revision not available"
    else
	echo "  newlib: not available"
    fi
    if test -f ${LOCAL_SRC_PATH}/comedi/revision.txt; then
	echo "  comedi: git from $(cat ${LOCAL_SRC_PATH}/comedi/revision.txt)"
    elif test -d ${LOCAL_SRC_PATH}/comedi; then
	echo "  comedi: revision not available"
    else
	echo "  comedi: not available"
    fi
}

function print_info {
    echo
    echo "loaded modules (lsmod):"
    if test -f lsmod.dat; then
	cat lsmod.dat
	rm -f lsmod.dat
    else
	lsmod
    fi
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
    print_versions
    echo
    echo "Hostname:"
    hostname
    echo
    echo "CPU (/proc/cpuinfo):"
    grep "model name" /proc/cpuinfo | head -n 1
    echo "$CPU_NUM cores"
    echo "machine (uname -m): $MACHINE"
    echo "$(free -h | grep Mem | awk '{print $2}') RAM"
    echo
    print_grub
    echo
    echo "settings of ${MAKE_RTAI_KERNEL}:"
    echo "KERNEL_PATH=$KERNEL_PATH"
    echo "LINUX_KERNEL=$LINUX_KERNEL"
    echo "KERNEL_SOURCE_NAME=$KERNEL_SOURCE_NAME"
    echo "KERNEL_NAME=$KERNEL_NAME"
    echo "LOCAL_SRC_PATH=$LOCAL_SRC_PATH"
    echo "RTAI_DIR=$RTAI_DIR"
    echo "RTAI_PATCH=$RTAI_PATCH"
    echo "SHOWROOM_DIR=$SHOWROOM_DIR"
    echo "KERNEL_CONFIG=$KERNEL_CONFIG"
    echo "RUN_LOCALMOD=$RUN_LOCALMOD"
    echo "KERNEL_DEBUG=$KERNEL_DEBUG"
}

function print_full_info {
    ORIG_RTAI_PATCH="$RTAI_PATCH"
    RTAI_PATCH=""
    check_kernel_patch
    if test $? -lt 10; then
	rm -f "$LOG_FILE"
    fi
    if test "x$1" != "xrtai"; then
	RTAI_PATCH="$ORIG_RTAI_PATCH"
	rm -f lsmod.dat
	print_info
    fi
}


###########################################################################
# packages:
function install_packages {
    # required packages:
    echo_log "install packages:"
    if ! command -v apt-get; then
	echo_log "Error: apt-get command not found!"
	echo_log "You are probably not on a Debian based Linux distribution."
	echo_log "The $MAKE_RTAI_KERNEL script will not work properly."
	echo_log "Exit"
	return 1
    fi
    PACKAGES="make gcc libncurses-dev zlib1g-dev g++ bc cvs git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev"
    if test ${LINUX_KERNEL:0:1} -gt 3; then
	PACKAGES="$PACKAGES libssl-dev libpci-dev libsensors4-dev"
    fi
    if $DRYRUN; then
	echo_log "apt-get -y install $PACKAGES"
	echo_log "apt-get -y install kernel-package"
    else
	if ! apt-get -y install $PACKAGES; then
	    echo_log "Failed to install missing packages!"
	    echo_log "Maybe some package names have changed..."
	    echo_log "We need the following packes, try to install them manually:"
	    for p in $PACKAGES; do
		echo_log "  $p"
	    done
	    return 1
	fi
	if ! apt-get -y install kernel-package; then
	    echo_log "Package kernel-package not availabel."
	fi
    fi
}

###########################################################################
# linux kernel:

function check_kernel_patch {
    if test -z "$RTAI_PATCH"; then
	if ! test -d "${LOCAL_SRC_PATH}/${RTAI_DIR}"; then
	    echo_log
	    echo_log "Error: RTAI source directory ${LOCAL_SRC_PATH}/${RTAI_DIR} does not exist."
	    echo_log "Download RTAI sources by running"
	    echo_log "$ ${MAKE_RTAI_KERNEL} download rtai"
	    return 10
	fi
	cd ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/
	echo_log
	echo_log "Available patches for this machine ($RTAI_MACHINE), most latest last:"
	ls -rt -1 *.patch 2> /dev/null | tee -a "$LOG_FILE"
	echo_log
	LINUX_KERNEL_V=${LINUX_KERNEL%.*}
	echo_log "Available patches for the selected kernel's kernel version ($LINUX_KERNEL_V):"
	ls -rt -1 *-${LINUX_KERNEL_V}*.patch 2> /dev/null | tee -a "$LOG_FILE"
	echo_log
	echo_log "Available patches for the selected kernel ($LINUX_KERNEL):"
	ls -rt -1 *-${LINUX_KERNEL}*.patch 2> /dev/null | tee -a "$LOG_FILE"
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
	echo_log
	echo_log "Currently running kernel:"
	echo_log $(uname -r)
	echo_log
	echo_log "Choose a patch and set the RTAI_PATCH variable at the top of the script"
	echo_log "and the LINUX_KERNEL variable with the corresponding kernel version."
	echo_log
	echo_log "Suggested values:"
	echo_log
	echo_log "RTAI_PATCH=\"${RTAI_PATCH}\""
	echo_log "LINUX_KERNEL=\"${LINUX_KERNEL}\""
	echo_log
	return 1
    elif ! test -f ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/$RTAI_PATCH; then
	echo_log
	echo_log "Error: rtai patch file $RTAI_PATCH does not exist."
	echo_log "Run again with -p \"\" to see list of available patches."
	return 2
    elif ! expr match $RTAI_PATCH ".*$LINUX_KERNEL" > /dev/null; then
	echo_log
	echo_log "Error: kernel version ${LINUX_KERNEL} does not match rtai patch ${RTAI_PATCH}."
	echo_log "Specify a matching kernel with the -k option or by setting the LINUX_KERNEL variable."
	echo_log
	if test "x${RTAI_PATCH:0:10}" = "xhal-linux-"; then
	    LINUX_KERNEL=${RTAI_PATCH#hal-linux-}
	    LINUX_KERNEL=${LINUX_KERNEL%%-*}
	    echo_log "Suggested value:"
	    echo_log
	    echo_log "LINUX_KERNEL=\"${LINUX_KERNEL}\""
	    echo_log
	fi
	return 2
    fi
    return 0
}

function download_kernel {
    if ! test -d "$KERNEL_PATH"; then
	echo_log "path to kernel sources $KERNEL_PATH does not exist!"
	return 1
    fi
    cd $KERNEL_PATH
    if ! check_kernel_patch; then
	return 1
    fi
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo_log "keep already downloaded linux kernel archive"
    elif test -n "$LINUX_KERNEL"; then
	echo_log "download linux kernel version $LINUX_KERNEL"
	if ! $DRYRUN; then
	    wget https://www.kernel.org/pub/linux/kernel/v${LINUX_KERNEL:0:1}.x/linux-$LINUX_KERNEL.tar.xz
	fi
    else
	echo_log
	echo_log "Available patches for this machine ($RTAI_MACHINE):"
	ls -rt ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/*.patch | while read LINE; do echo_log "  ${LINE#${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/}"; done
	echo_log
	echo_log "You need to specify a linux kernel version!"
	echo_log "Choose one from the above list of available rtai patches (most recent one is at the bottom)"
	echo_log "and pass it to this script via the -k option or set the LINUX_KERNEL variable directly."
	return 1
    fi
}

function unpack_kernel {
    if ! test -d "$KERNEL_PATH"; then
	echo_log "path to kernel sources $KERNEL_PATH does not exist!"
	return 1
    fi
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo_log "keep already existing linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} directory."
	echo_log "  remove it with $ ${MAKE_RTAI_KERNEL} clean kernel"
    else
	if ! test -f linux-$LINUX_KERNEL.tar.xz; then
	    echo_log "archive linux-$LINUX_KERNEL.tar.xz not found."
	    echo_log "download it with $ ${MAKE_RTAI_KERNEL} download kernel."
	fi
	# unpack:
	echo_log "unpack kernel sources from archive"
	if ! $DRYRUN; then
	    tar xof linux-$LINUX_KERNEL.tar.xz
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
	    return 1
	fi
	echo_log "apply rtai patch $RTAI_PATCH to kernel sources"
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
	echo_log "install kernel from debian package $KERNEL_PACKAGE"
	if ! $DRYRUN; then
	    dpkg -i "$KERNEL_PACKAGE"
	    if $KERNEL_DEBUG; then
		dpkg -i "$KERNEL_DEBUG_PACKAGE"
	    fi
	    GRUBMENU="$(grep '^menuentry' /boot/grub/grub.cfg | cut -d "'" -f 2 | grep "${LINUX_KERNEL}.*-${KERNEL_NAME}" | head -n 1)"
	    grub-reboot "$GRUBMENU"
	fi
    else
	echo_log "no kernel to install"
	return 1
    fi
}

function reboot_kernel {
    if test -z "$1"; then
	echo_log "reboot into ${LINUX_KERNEL}*-${KERNEL_NAME} kernel"
	if ! $DRYRUN; then
	    GRUBMENU="$(grep '^menuentry' /boot/grub/grub.cfg | cut -d "'" -f 2 | grep "${LINUX_KERNEL}.*-${KERNEL_NAME} " | head -n 1)"
	    grub-reboot "$GRUBMENU"
	    reboot
	fi
    else
	echo_log "reboot into grub menu $1"
	if ! $DRYRUN; then
	    grub-reboot "$@"
	    reboot
	fi
    fi
}

function build_kernel {
    cd /usr/src/linux
    echo_log "check for make-kpkg"
    HAVE_MAKE_KPKG=false
    if make-kpkg --help &> /dev/null; then
	HAVE_MAKE_KPKG=true
	echo_log "make-kpkg is available"
    fi
    if $NEW_KERNEL || $NEW_KERNEL_CONFIG || $RECONFIGURE_KERNEL; then

	if ! $RECONFIGURE_KERNEL; then
	    # clean:
	    echo_log "clean kernel sources"
	    if ! $DRYRUN; then
		if $HAVE_MAKE_KPKG; then
		    make-kpkg clean
		else
		    make clean
		fi
	    fi
	fi

	if $NEW_KERNEL_CONFIG; then
	    # kernel configuration:
	    if test "x$KERNEL_CONFIG" = "xdef"; then
		echo_log "use default configuration"
		if ! $DRYRUN; then
		    make defconfig
		fi
	    elif test "x$KERNEL_CONFIG" = "xold"; then
		echo_log "use configuration from running kernel (/boot/config-${CURRENT_KERNEL})"
		if ! $DRYRUN; then
		    cp /boot/config-${CURRENT_KERNEL} .config
		    make olddefconfig
		fi
	    elif test "x$KERNEL_CONFIG" = "xmod"; then
		echo_log "run make localmodconfig"
		if test ${CURRENT_KERNEL:0:${#LINUX_KERNEL}} != $LINUX_KERNEL ; then
		    echo_log "warning: kernel versions do not match (selected kernel is $LINUX_KERNEL, running kernel is $CURRENT_KERNEL)!"
		    echo_log "run make localmodconfig anyways"
		fi
		if ! $DRYRUN; then
		    make localmodconfig
		fi
		RUN_LOCALMOD=false
	    elif test -f "$KERNEL_CONFIG"; then
		echo_log "use configuration from $KERNEL_CONFIG"
		if ! $DRYRUN; then
		    cp "$KERNEL_CONFIG" .config
		    make olddefconfig
		fi
	    else
		echo_log "Unknown kernel configuration $KERNEL_CONFIG."
		return 0
	    fi
	    if $RUN_LOCALMOD; then
		if test ${CURRENT_KERNEL%.*} = ${LINUX_KERNEL%.*} ; then
		    echo_log "run make localmodconfig"
		    if ! $DRYRUN; then
			yes "" | make localmodconfig
		    fi
		else
		    echo_log "cannot run make localmodconfig, because kernel version does not match (running kernel: ${CURRENT_KERNEL}, selected kernel: ${LINUX_KERNEL})"
		fi
	    fi
	else
	    echo_log "keep already existing .configure file for linux-${LINUX_KERNEL}-${KERNEL_NAME}."
	fi

	# build the kernel:
	echo_log "build the kernel"
	if ! $DRYRUN; then
	    export CONCURRENCY_LEVEL=$CPU_NUM
	    if $HAVE_MAKE_KPKG; then
		if $KERNEL_DEBUG; then
		    make-kpkg --initrd --append-to-version -${KERNEL_NAME} --revision 1.0 --config menuconfig kernel_image kernel_debug
		else
		    make-kpkg --initrd --append-to-version -${KERNEL_NAME} --revision 1.0 --config menuconfig kernel-image
		fi
	    else
		make menuconfig
		make deb-pkg LOCALVERSION=-${KERNEL_NAME} KDEB_PKGVERSION=$(make kernelversion)-1
		# [TAR] creates a tar archive of the sources at the root of the kernel source tree
	    fi
 	    if test "x$?" != "x0"; then
		echo_log
		echo_log "Error: failed to build the kernel!"
		echo_log "Scroll up to see why."
		return 1
	    fi
	fi

	# install:
	install_kernel || return 1
    else
	echo_log "keep already compiled linux ${LINUX_KERNEL}-${KERNEL_NAME} kernel."
    fi
}

function clean_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo_log "remove kernel sources $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
	if ! $DRYRUN; then
	    rm -r linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	fi
    fi
}

function uninstall_kernel {
    # kernel:
    if test ${CURRENT_KERNEL} = ${LINUX_KERNEL}-${KERNEL_NAME} -o ${CURRENT_KERNEL} = ${LINUX_KERNEL}.0-${KERNEL_NAME}; then
	echo_log "Cannot uninstall a running kernel!"
	echo_log "First boot into a different kernel. E.g. by executing"
	echo_log "$ sudo ./${MAKE_RTAI_KERNEL} reboot"
	return 1
    fi
    echo_log "remove comedi kernel modules"
    if ! $DRYRUN; then
	rm -rf /lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}/comedi
    fi
    echo_log "uninstall kernel ${LINUX_KERNEL}-${KERNEL_NAME}"
    if ! $DRYRUN; then
	apt-get -y remove linux-image-${LINUX_KERNEL}-${KERNEL_NAME}
	apt-get -y remove linux-image-${LINUX_KERNEL}.0-${KERNEL_NAME}
    fi
    cd /usr/src/linux
}

function remove_kernel {
    cd $KERNEL_PATH
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo_log "remove kernel package $KERNEL_PATH/linux-$LINUX_KERNEL.tar.xz"
	if ! $DRYRUN; then
	    rm linux-$LINUX_KERNEL.tar.xz
	fi
    fi
    KERNEL_PACKAGE=$(ls linux-image-${LINUX_KERNEL}-${KERNEL_NAME}*.deb | tail -n 1)
    if test -f "$KERNEL_PACKAGE"; then
	echo_log "remove kernel package $KERNEL_PACKAGE"
	if ! $DRYRUN; then
	    rm "$KERNEL_PACKAGE"
	fi
    fi
}

function run_test {
    DIR=$1
    TEST=$2
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
    echo
}

function test_rtaikernel {
    if test ${CURRENT_KERNEL} != ${LINUX_KERNEL}-${KERNEL_NAME} -a ${CURRENT_KERNEL} != ${LINUX_KERNEL}.0-${KERNEL_NAME}; then
	echo "Need a running rtai kernel that matches the configuration of ${MAKE_RTAI_KERNEL}!"
	echo
	echo "Either boot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel, e.g. by executing"
	echo "$ sudo ./${MAKE_RTAI_KERNEL} reboot"
	echo "or supply the right parameter to ${MAKE_RTAI_KERNEL}."
	echo
	echo "Info: your running kernel is: ${CURRENT_KERNEL}"
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
	N="${LASTREPORT%%-*}"
	N=$(expr $N + 1)
	NUM="$(printf "%03d" $N)"
    fi

    # remove latency file to force calibration:
    # this is for rtai5, for rtai4 the calibration tools needs to be run manually
    # see base/arch/x86/calibration/README
    #if test -f /usr/realtime/calibration/latencies; then
    #    rm /usr/realtime/calibration/latencies
    #fi
    rm -f lsmod.dat

    # unload already loaded rtai kernel modules:
    lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed already loaded rtai_math"; }
    lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed already loaded rtai_sched"; }
    lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed already loaded rtai_hal"; }

    # loading rtai kernel modules:
    RTAIMOD_FAILED=false
    RTAIMATH_FAILED=false
    lsmod | grep -q rtai_hal || { insmod /usr/realtime/modules/rtai_hal.ko && echo "loaded rtai_hal" || RTAIMOD_FAILED=true; }
    lsmod | grep -q rtai_sched || { insmod /usr/realtime/modules/rtai_sched.ko && echo "loaded rtai_sched" || RTAIMOD_FAILED=true; }
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

	lsmod > lsmod.dat

	# remove comedi modules:
	modprobe -r kcomedilib && echo "removed kcomedilib"
	for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	    modprobe -r $i && echo "removed $i"
	done
	modprobe -r comedi && echo "removed comedi"
    fi

    # remove rtai modules:
    lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed rtai_math"; }
    lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed rtai_sched"; }
    lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed rtai_hal"; }

    if $RTAIMOD_FAILED; then
	echo "Failed to load RTAI modules."
	echo
	read -p 'Please enter a short name describing the configuration (empty: delete): ' NAME
	if test -n "$NAME"; then
	    REPORT="${LINUX_KERNEL}-${RTAI_DIR}-${NUM}-${NAME}-rtai-modules-failed"
	    {
		echo "failed to load rtai modules"
		echo
		print_versions
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
	    echo "saved kernel configuration in: config-$REPORT"
	    echo "saved test results in        : latencies-$REPORT"
	fi
	return
    fi
    echo "successfully loaded and unloaded rtai modules"
    echo

    # kernel tests:
    if test "x$1" != "xnone"; then
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
	echo "finished all tests"
	echo

	# report:
	read -p 'Please enter a short name describing the configuration (empty: delete): ' NAME
	if test -n "$NAME"; then
	    TEST_RESULT=""
	    TEST_DATA=$(grep RTD results-kern-latency.dat | tail -n 1)
	    OVERRUNS=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{print $7}')
	    if test "$OVERRUNS" -gt "0"; then
		TEST_RESULT="bad"
	    else
		LAT_MIN=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{print $3}')
		LAT_MAX=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{print $6}')
		LATENCY=$(( $LAT_MAX - $LAT_MIN ))
		if test "$LATENCY" -gt 15000; then
		    TEST_RESULT="bad"
		elif test "$LATENCY" -gt 5000; then
		    TEST_RESULT="ok"
		elif test "$LATENCY" -gt 1000; then
		    TEST_RESULT="good"
		else
		    TEST_RESULT="perfect"
		fi
	    fi
	    read -p "Please enter a short description of the test result (empty: $TEST_RESULT): " RESULT
	    if test -z "$RESULT"; then
		RESULT="$TEST_RESULT"
	    fi
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
	    echo "saved kernel configuration in : config-$REPORT"
	    echo "saved test results in         : latencies-$REPORT"
	fi
    else
	echo
	echo "test results not saved"
    fi
}


###########################################################################
# newlib:

function download_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib/src/newlib; then
	echo_log "keep already downloaded newlib sources"
    else
	echo_log "download newlib"
	if ! $DRYRUN; then
	    mkdir newlib
	    cd newlib
	    if git clone git://sourceware.org/git/newlib-cygwin.git src; then
		echo_log "downloaded newlib from git repository"
		date +"%F %H:%M" > src/revision.txt
		mkdir install
	    elif wget ftp://sourceware.org/pub/newlib/$NEWLIB_TAR; then
		echo_log "downloaded newlib snapshot $NEWLIB_TAR"
		tar xzf $NEWLIB_TAR
		NEWLIB_DIR=${NEWLIB_TAR%.tar.gz}
		mv $NEWLIB_DIR src
		echo ${NEWLIB_DIR#newlib-} > src/revision.txt
		mkdir install
	    fi
	fi
    fi
}

function update_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib/src/.git; then
	echo_log "update already downloaded newlib sources"
	cd newlib/src
	git pull origin master
	date +"%F %H:%M" > revision.txt
	clean_newlib
    elif ! test -f newlib/$NEWLIB_TAR; then
	rm -r newlib
	download_newlib
    fi
}

function install_newlib {
    cd ${LOCAL_SRC_PATH}/newlib
    cd install
    echo_log "install newlib"
    if ! $DRYRUN; then
	make install
	if test "x$?" != "x0"; then
	    echo_log "Failed to install newlib!"
	    return 1
	fi
    fi
    NEW_NEWLIB=true
}

function build_newlib {
    cd ${LOCAL_SRC_PATH}/newlib/install
    if test -f $MACHINE/lib/libm.a; then
	echo_log "keep already built and installed newlib library"
    else
	echo_log "build newlib"
	if ! $DRYRUN; then
	    NEWLIB_CFLAGS=""
	    if test "$(grep CONFIG_64BIT /usr/src/linux/.config)" = 'CONFIG_64BIT=y'; then
		NEWLIB_CFLAGS="$NEWLIB_CFLAGS -mcmodel=kernel"
	    fi
	    ${LOCAL_SRC_PATH}/newlib/src/newlib/configure --prefix=${LOCAL_SRC_PATH}/newlib/install --disable-shared --disable-multilib --target="$MACHINE" CFLAGS="${NEWLIB_CFLAGS}"
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo_log "Failed to build newlib!"
		return 1
	    fi
	    install_newlib || return 1
	fi
	NEW_NEWLIB=true
    fi
}

function clean_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo_log "clean newlib"
	if ! $DRYRUN; then
	    rm -rf newlib/install/*
	    cd newlib/src/newlib
	    make distclean
	fi
    fi
}

function uninstall_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo_log "uninstall newlib"
	if ! $DRYRUN; then
	    rm -r newlib/install/*
	fi
    fi
}

function remove_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib; then
	echo_log "remove ${LOCAL_SRC_PATH}/newlib"
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
	echo_log "keep already downloaded rtai sources"
	cd $RTAI_DIR
	echo_log "run make distclean on rtai sources"
	if ! $DRYRUN; then
	    make distclean
	fi
	cd -
    else
	echo_log "download rtai sources $RTAI_DIR"
	if ! $DRYRUN; then
	    if test "x$RTAI_DIR" = "xmagma"; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $RTAI_DIR
	    elif test "x$RTAI_DIR" = "xvulcano"; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $RTAI_DIR
	    elif test "x$RTAI_DIR" = "xRTAI"; then
		git clone https://github.com/ShabbyX/RTAI.git
	    else
		wget https://www.rtai.org/userfiles/downloads/RTAI/${RTAI_DIR}.tar.bz2
		echo_log "unpack ${RTAI_DIR}.tar.bz2"
		tar xof ${RTAI_DIR}.tar.bz2
		# -o option because we are root and want the files to be root!
	    fi
	    date +"%F %H:%M" > $RTAI_DIR/revision.txt
	fi
    fi
    echo_log "set soft link rtai -> $RTAI_DIR"
    if ! $DRYRUN; then
	ln -sfn $RTAI_DIR rtai
    fi
}

function update_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	cd $RTAI_DIR
	echo_log "run make distclean on rtai sources"
	if ! $DRYRUN; then
	    make distclean
	fi
	if test -d CVS; then
	    echo_log "update already downloaded rtai sources"
	    if ! $DRYRUN; then
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai update
		date +"%F %H:%M" > revision.txt
	    fi
	elif test -d .git; then
	    echo_log "update already downloaded rtai sources"
	    if ! $DRYRUN; then
		git pull origin master
		date +"%F %H:%M" > revision.txt
	    fi
	elif test -f ../${RTAI_DIR}.tar.bz2; then
	    cd -
	    echo_log "unpack ${RTAI_DIR}.tar.bz2"
	    tar xof ${RTAI_DIR}.tar.bz2
	    cd -
	fi
	cd -
	echo_log "set soft link rtai -> $RTAI_DIR"
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
	echo_log "build rtai"
	if ! $DRYRUN; then
	    # path to newlib math library:
	    LIBM_PATH=$(find ${LOCAL_SRC_PATH}/newlib/install/ -name 'libm.a' | head -n 1)
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
	    if grep -q 'CONFIG_RTAI_VERSION="5' .rtai_config; then
		# ./configure script options seem to be very outdated (new libmath support)! 
		# So, the following won't work:
		# ./configure --enable-cpus=${RTAI_NUM_CPUS} --enable-fpu --with-math-libm-dir=$LIBM_PATH
		patch <<EOF
--- base/arch/x86/defconfig     2017-11-17 16:20:00.000000000 +0100
+++ .rtai_config        2018-03-14 18:03:41.700624880 +0100
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
@@ -38,8 +39,12 @@
 #
 # CONFIG_RTAI_SCHED_ISR_LOCK is not set
 # CONFIG_RTAI_LONG_TIMED_LIST is not set
-CONFIG_RTAI_LATENCY_SELF_CALIBRATION_FREQ="10000"
-CONFIG_RTAI_LATENCY_SELF_CALIBRATION_CYCLES="10000"
+# CONFIG_RTAI_USE_STACK_ARGS is not set
+CONFIG_RTAI_LATENCY_SELF_CALIBRATION_METRICS="1"
+CONFIG_RTAI_LATENCY_SELF_CALIBRATION_FREQ="10000"
+CONFIG_RTAI_LATENCY_SELF_CALIBRATION_CYCLES="10000"
+CONFIG_RTAI_KERN_BUSY_ALIGN_RET_DELAY="0"
+CONFIG_RTAI_USER_BUSY_ALIGN_RET_DELAY="0"
 CONFIG_RTAI_SCHED_LXRT_NUMSLOTS="150"
 CONFIG_RTAI_MONITOR_EXECTIME=y
 CONFIG_RTAI_ALLOW_RR=y
@@ -69,7 +74,10 @@
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
	    if ! ${MAKE_NEWLIB}; then
		patch <<EOF
--- .rtai_config_math   2018-03-14 16:29:57.156483235 +0100
+++ .rtai_config        2018-03-14 16:30:24.116483914 +0100
@@ -74,10 +74,7 @@
 # Other features
 #
 CONFIG_RTAI_USE_NEWERR=y
-CONFIG_RTAI_MATH=y
-CONFIG_RTAI_MATH_LIBM_TO_USE="1"
-CONFIG_RTAI_MATH_LIBM_DIR=""
-# CONFIG_RTAI_MATH_KCOMPLEX is not set
+# CONFIG_RTAI_MATH is not set
 CONFIG_RTAI_MALLOC=y
 # CONFIG_RTAI_USE_TLSF is not set
 CONFIG_RTAI_MALLOC_VMALLOC=y
EOF
	    fi
	    make -f makefile oldconfig
	    if $RTAI_MENU; then
		make menuconfig
	    fi
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo_log "Failed to build rtai modules!"
		return 1
	    fi
	    install_rtai || return 1
	fi
	NEW_RTAI=true
    else
	echo_log "keep already built rtai modules"
    fi
}

function clean_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	echo_log "clean rtai"
	if ! $DRYRUN; then
	    cd $RTAI_DIR
	    make clean
	fi
    fi
}

function install_rtai {
    cd ${LOCAL_SRC_PATH}/${RTAI_DIR}
    echo_log "install rtai"
    if ! $DRYRUN; then
	make install
	if test "x$?" != "x0"; then
	    echo_log "Failed to install rtai modules!"
	    return 1
	fi
    fi
    NEW_RTAI=true
}

function uninstall_rtai {
    if test -d /usr/realtime; then
	echo_log "uninstall rtai"
	if ! $DRYRUN; then
	    rm -r /usr/realtime
	fi
    fi
}

function remove_rtai {
    cd ${LOCAL_SRC_PATH}
    if test -d $RTAI_DIR; then
	echo_log "remove rtai in ${LOCAL_SRC_PATH}/$RTAI_DIR"
	if ! $DRYRUN; then
	    rm -r $RTAI_DIR
	fi
    fi
    if test -f $RTAI_DIR.tar.*; then
	echo_log "remove ${LOCAL_SRC_PATH}/$RTAI_DIR.tar.*"
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
	echo_log "keep already downloaded rtai-showroom sources"
	cd $SHOWROOM_DIR/v3.x
	echo_log "run make clean on rtai-showroom sources"
	if ! $DRYRUN; then
	    PATH="$PATH:/usr/realtime/bin"
	    make clean
	fi
	cd -
    else
	echo_log "download rtai-showroom sources"
	if ! $DRYRUN; then
	    cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $SHOWROOM_DIR
	    date +"%F %H:%M" > $SHOWROOM_DIR/revision.txt
	fi
    fi
}

function update_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo_log "update already downloaded rtai-showroom sources"
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
    echo_log "build rtai showroom"
    if ! $DRYRUN; then
	PATH="$PATH:/usr/realtime/bin"
	make
	if test "x$?" != "x0"; then
	    echo_log "Failed to build rtai showroom!"
	    return 1
	fi
    fi
}

function clean_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo_log "clean rtai showroom"
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
	echo_log "remove rtai showroom in ${LOCAL_SRC_PATH}/$SHOWROOM_DIR"
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
	echo_log "keep already downloaded comedi sources"
    else
	echo_log "download comedi"
	if ! $DRYRUN; then
	    git clone https://github.com/Linux-Comedi/comedi.git
	    date +"%F %H:%M" > comedi/revision.txt
	fi
    fi
}

function update_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo_log "update already downloaded comedi sources"
	cd comedi
	git pull origin master
	date +"%F %H:%M" > revision.txt
	clean_comedi
    else
	download_comedi
    fi
}

function install_comedi {
    echo_log "remove all loaded comedi kernel modules"
    if ! $DRYRUN; then
	modprobe -r kcomedilib && echo_log "removed kcomedilib"
	for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	    modprobe -r $i && echo_log "removed $i"
	done
	modprobe -r comedi && echo_log "removed comedi"
    fi

    echo_log "remove comedi staging kernel modules"
    if ! $DRYRUN; then
	rm -rf /lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}/kernel/drivers/staging/comedi
    fi

    cd ${LOCAL_SRC_PATH}/comedi
    echo_log "install comedi"
    if ! $DRYRUN; then
	make install
	if test "x$?" != "x0"; then
	    echo_log "Failed to install comedi!"
	    return 1
	fi
	KERNEL_MODULES=/lib/modules/${LINUX_KERNEL}-${KERNEL_NAME}
	if ! test -d "$KERNEL_MODULES"; then
	    KERNEL_MODULES=/lib/modules/${LINUX_KERNEL}.0-${KERNEL_NAME}
	fi
	cp ${LOCAL_SRC_PATH}/comedi/comedi/Module.symvers ${KERNEL_MODULES}/comedi/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedi.h /usr/include/linux/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedilib.h /usr/include/linux/
	depmod -a
	sleep 1
	udevadm trigger
    fi
    NEW_COMEDI=true
}

function build_comedi {
    if $NEW_RTAI || ! test -f ${LOCAL_SRC_PATH}/comedi/comedi/comedi.o; then
	cd ${LOCAL_SRC_PATH}/comedi
	echo_log "build comedi"
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
		echo_log "Failed to build comedi!"
		return 1
	    fi
	    install_comedi || return 1
	fi
	NEW_COMEDI=true
    else
	echo_log "keep already installed comedi modules"
    fi
}

function clean_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo_log "clean comedi"
	cd comedi
	if ! $DRYRUN; then
	    make clean
	fi
    fi
}

function uninstall_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo_log "uninstall comedi"
	cd comedi
	if ! $DRYRUN; then
	    make uninstall
	fi
    fi
}

function remove_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo_log "remove ${LOCAL_SRC_PATH}/comedi"
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

    if ! install_packages; then
	return 1
    fi

    uninstall_kernel

    if ! ( ( ${MAKE_RTAI} && download_rtai || true ) && ( ${MAKE_NEWLIB} && download_newlib || true ) && ( ${MAKE_COMEDI} && download_comedi || true ) && download_kernel ); then
	echo_log "Failed to download some of the sources"
	return 1
    fi

    if ! ( unpack_kernel && patch_kernel && build_kernel ); then
	echo_log "Failed to patch and build the kernel"
	return 1
    fi

    if ! ( ( ${MAKE_NEWLIB} && build_newlib || MAKE_NEWLIB=false ) && ( ${MAKE_RTAI} && build_rtai || MAKE_RTAI=false ) && ( ${MAKE_COMEDI} && build_comedi || MAKE_COMEDI=false ) ); then
	echo_log "Failed to build newlib, RTAI, or comedi"
	return 1
    fi

    echo_log
    echo_log "Done!"
    echo_log "Please reboot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel"
    echo_log
}

function reconfigure {
    RECONFIGURE_KERNEL=true
    check_root

    uninstall_kernel
    ${MAKE_RTAI} && uninstall_rtai
    ${MAKE_COMEDI} && uninstall_comedi

    if ! ( unpack_kernel && patch_kernel && build_kernel && ( ${MAKE_NEWLIB} && build_newlib || MAKE_NEWLIB=false ) && ( ${MAKE_RTAI} && build_rtai || true ) && ( ${MAKE_COMEDI} && build_comedi || true ) ); then
	echo_log "Failed to reconfigure and build kernel, RTAI, or comedi"
	return 1
    fi

    echo_log
    echo_log "Done!"
    echo_log "Please reboot into the ${LINUX_KERNEL}-${KERNEL_NAME} kernel"
}

function download_all {
    check_root
    if test -z $1; then
	download_kernel
	${MAKE_NEWLIB} && download_newlib
	${MAKE_RTAI} && download_rtai
	${MAKE_COMEDI} && download_comedi
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
	${MAKE_NEWLIB} && update_newlib
	${MAKE_RTAI} && update_rtai
	${MAKE_COMEDI} && update_comedi
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
	unpack_kernel && patch_kernel && build_kernel && ( ${MAKE_NEWLIB} && build_newlib || MAKE_NEWLIB=false ) && ( ${MAKE_RTAI} && build_rtai || MAKE_RTAI=false ) && ( ${MAKE_COMEDI} && build_comedi || MAKE_COMEDI=false )
    else
	for TARGET; do
	    case $TARGET in
		kernel ) 
		    unpack_kernel && patch_kernel && build_kernel && ( ${MAKE_NEWLIB} && build_newlib || true ) && ( ${MAKE_RTAI} && build_rtai || true ) && ( ${MAKE_COMEDI} && build_comedi || true ) ;;
		newlib )
		    build_newlib && ( ${MAKE_RTAI} && build_rtai || true ) && ( ${MAKE_COMEDI} && build_comedi || true ) ;;
		rtai ) 
		    build_rtai && ( ${MAKE_COMEDI} && build_comedi || true ) ;;
		showroom ) 
		    build_showroom ;;
		comedi ) 
		    build_comedi ;;
	    esac
	done
    fi
}

function install_all {
    check_root
    if test -z "$1"; then
	install_packages
	install_kernel
	${MAKE_NEWLIB} && install_newlib
	${MAKE_RTAI} && install_rtai
	${MAKE_COMEDI} && install_comedi
    else
	for TARGET; do
	    case $TARGET in
		packages ) install_packages ;;
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
	${MAKE_NEWLIB} && clean_newlib
	${MAKE_RTAI} && clean_rtai
	${MAKE_COMEDI} && clean_comedi
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
	${MAKE_NEWLIB} && uninstall_newlib
	${MAKE_RTAI} && uninstall_rtai
	${MAKE_COMEDI} && uninstall_comedi
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
	${MAKE_NEWLIB} && remove_newlib
	${MAKE_RTAI} && remove_rtai
	${MAKE_COMEDI} && remove_comedi
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
   check_root && clean_kernel && unpack_kernel && patch_kernel
 }


###########################################################################
###########################################################################
# main script:

rm -f "$LOG_FILE"

if test "x$1" = "x-d"; then
    shift
    DRYRUN=true
fi
if test "x$1" = "x-s"; then
    shift
    if test -n "$1" && test "x$1" != "xreconfigure"; then
	KERNEL_PATH=$1
	shift
    else
	echo "you need to specify a path for the kernel sources after the -s option"
	exit 1
    fi
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
if test "x$1" = "x-l"; then
    shift
    RUN_LOCALMOD=false
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
    echo_log "Warning: you changed rtai sources and you might need to adapt the linux kernel version and rtai patch file to it."
    sleep 2
fi

ACTION=$1
shift
case $ACTION in

    help ) print_usage ;;
    --help ) print_usage ;;

    version ) print_version ;;
    --version ) print_version ;;

    info ) if test "x$1" = "xgrub"; then
	    print_grub
	else
	    print_full_info $@ 
	    if test "x$1" = "xrtai"; then
		rm -f "$LOG_FILE"
	    fi 
	fi ;;

    reconfigure ) reconfigure ;;

    test ) 
	test_rtaikernel $@
	exit ;;

    download ) download_all $@ ;;
    update ) update_all $@ ;;
    patch ) clean_unpack_patch_kernel ;;
    build ) build_all $@ ;;
    install ) install_all $@ ;;
    clean ) clean_all $@ ;;
    uninstall ) uninstall_all $@ ;;
    remove ) remove_all $@ ;;
    reboot ) reboot_kernel $@;;

    * ) if test -n "$1"; then
	    echo "unknown option \"$1\""
	    echo
	    print_usage
	    exit 1
	else
	    full_install
	fi ;;

esac

STATUS=$?

if test -f "$LOG_FILE"; then
    echo
    echo "Summary of log messages"
    echo "-----------------------"
    cat "$LOG_FILE"
fi

exit $STATUS
