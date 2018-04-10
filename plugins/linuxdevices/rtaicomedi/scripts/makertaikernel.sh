#!/bin/bash

###########################################################################
# you should modify the following parameter according to your needs:

: ${KERNEL_PATH:=/usr/src}       # where to put and compile the kernel (set with -s)
: ${LINUX_KERNEL:="4.4.115"}     # linux vanilla kernel version (set with -k)
: ${KERNEL_SOURCE_NAME:="rtai"}  # name for kernel source directory to be appended to LINUX_KERNEL
: ${KERNEL_NUM:="-1"}            # name of the linux kernel is $LINUX_KERNEL-$RTAI_DIR$KERNEL_NUM
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
: ${RTAI_PATCH:="hal-linux-4.4.115-x86-10.patch"} # rtai patch to be used (set with -p)

: ${KERNEL_CONFIG:="old"}  # whether and how to initialize the kernel configuration 
                           # (set with -c) 
                           # "old" for oldconfig from the running kernel,
                           # "def" for the defconfig target,
                           # "mod" for the localmodconfig target, (even if kernel do not match)
                           # or a full path to a config file.
                           # afterwards, the localmodconfig target is executed, 
                           # if the running kernel matches LINUX_KERNEL
: ${RUN_LOCALMOD:=true}    # run make localmodconf after selecting a kernel configuration (disable with -l)
: ${KERNEL_MENU:="menuconfig"} # the menu for editing the kernel configuration
                               # (menuconfig, gconfig, xconfig)
: ${KERNEL_DEBUG:=false}   # generate debugable kernel (see man crash), set with -D

: ${KERNEL_PARAM:="idle=poll nohz=off"}      # kernel parameter to be passed to grub
: ${KERNEL_PARAM_DESCR:="idle-nohz"}     # one-word description of KERNEL_PARAM 
                                    # used for naming test resutls
: ${BATCH_KERNEL_PARAM:="panic=10"} # additional kernel parameter passed to grub for test batch

: ${NEWLIB_TAR:=newlib-3.0.0.20180226.tar.gz}  # tar file of current newlib version 
                                               # at ftp://sourceware.org/pub/newlib/index.html
                                               # in case git does not work

: ${MAKE_NEWLIB:=true}       # for automatic targets make newlib library
: ${MAKE_RTAI:=true}         # for automatic targets make rtai library
: ${MAKE_COMEDI:=true}       # for automatic targets make comedi library

: ${RTAI_HAL_PARAM:=""}      # parameter for the rtai_hal module used for testing
: ${RTAI_SCHED_PARAM:=""}    # parameter for the rtai_sched module used for testing
: ${TEST_TIME:=""}           # time in seconds used for latency test

: ${SHOWROOM_DIR:=showroom}  # target directory for rtai-showrom in ${LOCAL_SRC_PATH}

: ${STARTUP_TIME:=180}       # time to wait after boot to run a batch test in seconds

: ${REMOTE_MACHINE:=}        # ip adress of a remote machine for ping flood

###########################################################################
# some global variables:

FULL_COMMAND_LINE="$@"
MAKE_RTAI_KERNEL="${0##*/}"

VERSION_STRING="${MAKE_RTAI_KERNEL} version 4.0 by Jan Benda, April 2018"
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

KERNEL_NAME=${LINUX_KERNEL}-${RTAI_DIR}${KERNEL_NUM}
KERNEL_ALT_NAME=${LINUX_KERNEL}.0-${RTAI_DIR}${KERNEL_NUM}
REALTIME_DIR="/usr/realtime"   
# this will be reset after reading in command line parameter!

LOG_FILE="${PWD}/${MAKE_RTAI_KERNEL%.*}.log"


###########################################################################
# general functions:

function echo_log {
    echo "$@" | tee -a "$LOG_FILE"
}

function echo_kmsg {
    # this is for dmesg:
    echo "#### MAKERTAIKERNEL.SH: $@" > /dev/kmsg
    # this goes into the logger files: 
    # (on some systems kmsg does not end up in logger files)
    logger -p user.info "#### MAKERTAIKERNEL.SH: $@"
}

function print_version {
    echo $VERSION_STRING
}

function help_kernel_options {
    cat <<EOF
-d    : dry run - only print out what the script would do, but do not execute any command
-k xxx: use linux kernel version xxx (LINUX_KERNEL=${LINUX_KERNEL})
-r xxx: the rtai source (RTAI_DIR=${RTAI_DIR}), one of
        magma: current rtai development version from csv
        vulcano: stable rtai development version from csv
        rtai-5.1: rtai release version 5.1 from www.rtai.org
        rtai-4.1: rtai release version 4.1 from www.rtai.org, or any other
        rtai-x.x: rtai release version x.x from www.rtai.org
        RTAI: snapshot from Shahbaz Youssefi's RTAI clone on github
-n xxx: append xxx to the name of the linux kernel (KERNEL_NUM=${KERNEL_NUM})
EOF
}

function help_info {
    cat <<EOF
$VERSION_STRING

Print some information about your system.

Usage:

sudo ${MAKE_RTAI_KERNEL} info [rtai|grub|setup]

info         : display properties of rtai patches, loaded kernel modules, kernel, machine,
               and grub menu
info rtai    : list all available patches and suggest the one fitting to the kernel
info settings: print the values of all configuration variables of the ${MAKE_RTAI_KERNEL} script
info grub    : show grub boot menu entries
info setup   : show modifikations of your system made by ${MAKE_RTAI_KERNEL} (run as root)

EOF
}

function help_setup {
    cat <<EOF
$VERSION_STRING

Setup and restore syslog daemon, grub menu, and kernel parameter.

Usage:

sudo ${MAKE_RTAI_KERNEL} setup [messages|grub|comedi|kernel]

sudo ${MAKE_RTAI_KERNEL} restore [messages|grub|comedi|kernel|testbatch]

setup            : setup messages, grub, comedi, and kernel
setup messages   : enable /var/log/messages needed for RTAI tests in rsyslog settings
setup grub       : configure the grub boot menu (not hidden, no submenus, 
                   extra RTAI kernel parameter, user can reboot)
setup comedi     : create "iocard" group and assign comedi devices to this group
setup kernel     : set kernel parameter for the grub boot menu to "$KERNEL_PARAMETER"

restore          : restore the original system settings (messages, grub, comedi, kernel, and testbatch)
restore messages : restore the original rsyslog settings
restore grub     : restore the original grub boot menu settings
restore comedi   : do not assign comedi devices to the iocard group
restore kernel   : restore default kernel parameter for the grub boot menu
restore testbatch: uninstall the automatic test script from crontab (see help test)

EOF
}

function help_reboot {
    cat <<EOF
$VERSION_STRING

Reboot and set kernel parameter.

Usage:

sudo ${MAKE_RTAI_KERNEL} [-d] [-n xxx] [-r xxx] [-k xxx] reboot [keep|default|<XXX>|<FILE>|<N>]

EOF
    help_kernel_options
    cat <<EOF

reboot        : reboot into the rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                with kernel parameter as specified by KERNEL_PARAM
                (currently set to "$KERNEL_PARAM")
reboot XXX    : reboot into rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                with XXX passed on as kernel parameter
reboot FILE   : reboot into rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                with kernel parameter taken from test results file FILE
reboot keep   : reboot into rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                and keep previously set kernel parameter
reboot default: reboot into rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                without additional kernel parameter
reboot N      : reboot into a kernel as specified by grub menu entry index N
                without additional kernel parameter,
                see "${MAKE_RTAI_KERNEL} info grub" for the grub menu.

EOF
}

function help_test {
    cat <<EOF
$VERSION_STRING

Test the performance of the rtai-patched linux kernel.

Usage:

sudo ${MAKE_RTAI_KERNEL} [-d] [-n xxx] [-r xxx] [-k xxx] test [[hal|sched|math|comedi] [calib]
     [kern|kthreads|user|all|none] [<NNN>] [auto <XXX> | batch basic|acpi|isolcpus|<FILE>]]

EOF
    help_kernel_options
    cat <<EOF

Tests are performed only if the running kernel matches the one
${MAKE_RTAI_KERNEL} is configured for.

Test resluts are saved in latencies-* files in the current working
directory. The corresponding kernel configuration is saved in the
respective config-* files.

First, loading and unloading of rtai and comedi modules is tested. 
This can be controlled by the following key-words of which one can be specified:
  hal     : test loading and unloading of rtai_hal kernel module only
  sched   : test loading and unloading of rtai_hal and rtai_sched kernel modules
  math    : test loading and unloading of rtai_hal, rtai_sched, and rtai_math kernel module
  comedi  : test loading and unloading of rtai and comedi kernel modules
Additionally, you may specify:
  calib   : force calibration of scheduling latencies (default is no calibration)

Then the rtai tests (latency, switch, and preempt) are executed. You can
select what to test by specifying one or more of the following key-words:
  kern     : run the kern tests (default)
  kthreads : run the kthreads tests
  user     : run the user tests
  all      : run the kernel, kthreads, and user tests
  none     : test loading and unloading of kernel modules and do not run any tests

You may want to run some load on you system to really test the RTAI
performance. This can be controlled by the following key-words:
  cpu      : run heavy computations on each core
  io       : do some heavy file reading and writing
  mem      : do some heavy memory access
  net      : produce network traffic
  full     : all of the above

The rtai tests need to be terminated by pressing ^C and a string
describing the test scenario needs to be provided. This can be
automized by the following two options:
  NNN      : the number of seconds after which the latency test is automatically aborted,
             the preempt test will be aborted after 10 seconds.
  auto XXX : a one-word description of the kernel configuration (no user interaction)

For a completely automized series of tests of various kernel parameters
under different loads you may add as the last arguments:
  batch FILE     : automatically run tests with various kernel parameter as specified in FILE
  batch basic    : write a default file with clock and timer related kernel parameters to be tested
  batch acpi     : write a default file with acpi and apic related kernel parameters to be tested
  batch isolcpus : write a default file with load settings and isolcpus kernel parameters to be tested
This successively reboots into the RTAI kernel with the kernel parameter 
set to the ones specified by the KERNEL_PARAM variable and as specified in FILE,
and runs the tests as specified by the previous commands (without the "auto" command).

In a batch FILE
- everything behind a hash ('#') is a comment that is completely ignored
- empty lines are ignored
- a line of the format
  <descr> : <load> : <params>
  describes a configuration to be tested:
  <descr> is a one-word string describing the kernel parameter 
      (the load settings are added automatically to the description)
  <load> defines the load processes to be started before testing (cpu io mem net full, see above)
  <param> is a list of kernel parameter to be used.
Example lines:
  idlenohz : : idle=poll nohz=off
  idleisol : cpu io : idle=poll isolcpus=0-1
See the file written by "batch default" for suggestions, and the file
$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/Documentation/kernel-parameters.txt
for a documentation of all kernel parameter.

EOF
}

function help_report {
    cat <<EOF
$VERSION_STRING

Generate summary tables of test results.

Usage:

${MAKE_RTAI_KERNEL} report [avg|max] [<FILES>]

The summary is written to standard out. For example, redirect the
output into a file to save the summary:

${MAKE_RTAI_KERNEL} report > testsummary.dat 

or pipe it into less to view the results:

${MAKE_RTAI_KERNEL} report | less -S

The tests can be sorted by providing the avg or max key-word:
report     : do not sort
report avg : sort the tests according to the avgmax field of the kern latency test
report max : sort the tests according to the ovlmax field of the kern latency test

The remaining arguments specify files to be included in the summary,
or a directory containing latency-* files. If no file is specified,
all latency-* files in the current directory are used.

EOF
}

function help_usage {
    cat <<EOF
$VERSION_STRING

Download, build, install and test everything needed for an rtai-patched linux kernel with math and comedi support.

usage:
sudo ${MAKE_RTAI_KERNEL} [-d] [-s xxx] [-n xxx] [-r xxx] [-p xxx] [-k xxx] [-c xxx] [-l] [-D] [-m] 
     [action [target1 [target2 ... ]]]

EOF
    help_kernel_options
    cat <<EOF
-s xxx: use xxx as the base directory where to put the kernel sources (KERNEL_PATH=${KERNEL_PATH})
-p xxx: use rtai patch file xxx (RTAI_PATCH=${RTAI_PATCH})
-c xxx: generate a new kernel configuration (KERNEL_CONFIG=${KERNEL_CONFIG}):
        old: use the kernel configuration of the currently running kernel
        def: generate a kernel configuration using make defconfig
        mod: simplify existing kernel configuration using make localmodconfig
             even if kernel do not match
        path/to/config/file: provide a particular configuration file
        After setting the configuration (except for mod), make localmodconfig
        is executed to deselect compilation of unused modules, but only if the
        runnig kernel matches the selected kernel version (major.minor only).
-l    : disable call to make localmodconf after a kernel configuration 
        has been selected via the -c switch (RUN_LOCALMOD=${RUN_LOCALMOD})
-D    : generate kernel package with debug symbols in addition (KERNEL_DEBUG=${KERNEL_DEBUG})
-m    : enter the RTAI configuration menu

If no action is specified, a full download and build is performed for all targets (except showroom).

For the targets one or more of:
  packages   : required packages (install only)
  kernel     : rtai-patched linux kernel
  newlib     : newlib library
  rtai       : rtai modules
  showroom   : rtai showroom examples (supports only download, build, clean, remove)
  comedi     : comedi data acquisition driver modules
action can be one of:
  download   : download missing sources of the specified targets
  update     : update sources of the specified targets (not for kernel target)
  patch      : clean, unpack, and patch the linux kernel with the rtai patch (no target required)
  prepare    : prepare kernel configs for a test batch (no target required)
  build      : compile and install the specified targets and the depending ones if needed
  buildplain : compile and install the kernel without the rtai patch (no target required)
  clean      : clean the source trees of the specified targets
  install    : install the specified targets
  uninstall  : uninstall the specified targets
  remove     : remove the complete source trees of the specified targets.
If no target is specified, all targets are made (except showroom).

Action can be also one of
  help       : display this help message
  help XXX   : display help message for action XXX
  info       : display properties of rtai patches, loaded kernel modules,
               kernel, machine, and grub menu
               (run "${MAKE_RTAI_KERNEL} help info" for details)
  init       : should be executed the first time you use ${MAKE_RTAI_KERNEL} -
               equivalent to setup, download rtai, info rtai.
  reconfigure: reconfigure the kernel and make a full build of all targets (without target)
  reboot     : reboot and set kernel parameter
               (run "${MAKE_RTAI_KERNEL} help reboot" for details)
  setup      : setup some basic configurations of your (debian based) system
               (run "${MAKE_RTAI_KERNEL} help setup" for details)
  restore    : restore the original system settings
               (run "${MAKE_RTAI_KERNEL} help restore" for details)
  test       : test the current kernel and write reports to the current working directory 
               (run "${MAKE_RTAI_KERNEL} help test" for details)
  report     : summarize test results from latencies* files given in FILES
               (run "${MAKE_RTAI_KERNEL} help report" for details)

Common use cases:

Start with setting up /var/log/messages, the grub boot menu, and
downloading an rtai source (-r option or RTAI_DIR variable):
$ sudo ${MAKE_RTAI_KERNEL} init

Select a Linux kernel and a RTAI patch from the displayed list and set
the LINUX_KERNEL and RTAI_PATCH variables in the makertaikernel.sh
script accordingly.

Check again for available patches:
$ sudo ${MAKE_RTAI_KERNEL} info rtai

Once you have decided on a patch and you have set LINUX_KERNEL and
RTAI_PATCH variables accrdingly run
$ sudo ${MAKE_RTAI_KERNEL}
to download and build all targets. A new configuration for the kernel is generated.

$ sudo ${MAKE_RTAI_KERNEL} test
  manually test the currently running kernel.

$ sudo ${MAKE_RTAI_KERNEL} test 30 auto basic
  automaticlly test the currently running kernel for 30 seconds and name it "basic".

$ sudo ${MAKE_RTAI_KERNEL} test 600 batch testbasic.mrk
  automaticlly test all the kernel paramete rspecified in the file testbasic.mrk.

$ ${MAKE_RTAI_KERNEL} report avg | less -S
  view test results sorted with respect to the averaged maximum latency. 

$ sudo ${MAKE_RTAI_KERNEL} reconfigure
  build all targets using the existing configuration of the kernel.

$ sudo ${MAKE_RTAI_KERNEL} uninstall
  uninstall all targets.
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

function print_setup {
    check_root

    echo
    echo "System modifications of ${MAKE_RTAI_KERNEL}:"
    echo
    # messages:
    if test -f /etc/rsyslog.d/50-default.conf.origmrk; then
	echo "messages : /etc/rsyslog.d/50-default.conf is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore messages\" to restore"
    else
	echo "messages : /etc/rsyslog.d/50-default.conf is not modified"
	echo "           run \"${MAKE_RTAI_KERNEL} setup messages\" for setting up"
    fi
    # grub:
    if test -f /etc/default/grub.origmrk; then
	echo "grub     : /etc/default/grub is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore grub\" to restore"
    else
	echo "grub     : /etc/default/grub is not modified"
	echo "           run \"${MAKE_RTAI_KERNEL} setup grub\" for setting up"
    fi
    if test -f /etc/grub.d/10_linux.origmrk; then
	echo "grub     : /etc/grub.d/10_linux is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore grub\" to restore"
    else
	echo "grub     : /etc/grub.d/10_linux is not modified"
	echo "           run \"${MAKE_RTAI_KERNEL} setup grub\" for setting up"
    fi
    # kernel parameter:
    if test -f /etc/default/grub.origkp; then
	echo "kernel   : /etc/default/grub is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore kernel\" to restore"
    else
	echo "kernel   : /etc/default/grub is not modified"
	echo "           run \"${MAKE_RTAI_KERNEL} setup kernel\" for setting up"
    fi
    # test batch:
    if crontab -l 2> /dev/null | grep -q "${MAKE_RTAI_KERNEL}"; then
	echo "testbatch: crontab is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore testbatch\" to restore"
    else
	echo "testbatch: crontab is not modified"
    fi
    echo
}

function print_grub {
    echo "grub menu entries:"
    IFSORG="$IFS"
    IFS=$'\n'
    N=0
    for gm in $(grep '^\s*menuentry ' /boot/grub/grub.cfg | cut -d "'" -f 2); do
	echo "  $N) $gm"
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
    elif test -f ${LOCAL_SRC_PATH}/newlib/revision.txt; then
	echo "  newlib: git from $(cat ${LOCAL_SRC_PATH}/newlib/revision.txt)"
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

function print_distribution {
    if lsb_release &> /dev/null; then
	echo "distribution (lsb_release -a):"
	lsb_release -a 2> /dev/null | while read LINE; do echo "  $LINE"; done
    else
	echo "distribution: unknown"
    fi
}

function print_settings {
    echo "settings of ${VERSION_STRING}:"
    echo "  KERNEL_PATH   (-s) = $KERNEL_PATH"
    echo "  LINUX_KERNEL  (-k) = $LINUX_KERNEL"
    echo "  KERNEL_SOURCE_NAME = $KERNEL_SOURCE_NAME"
    echo "  KERNEL_NUM    (-n) = $KERNEL_NUM"
    echo "  KERNEL_CONFIG (-c) = $KERNEL_CONFIG"
    echo "  KERNEL_MENU         = $KERNEL_MENU"
    echo "  RUN_LOCALMOD  (-l) = $RUN_LOCALMOD"
    echo "  KERNEL_DEBUG  (-D) = $KERNEL_DEBUG"
    echo "  KERNEL_PARAM       = $KERNEL_PARAM"
    echo "  KERNEL_PARAM_DESCR = $KERNEL_PARAM_DESCR"
    echo "  LOCAL_SRC_PATH     = $LOCAL_SRC_PATH"
    echo "  RTAI_DIR      (-r) = $RTAI_DIR"
    echo "  RTAI_PATCH    (-p) = $RTAI_PATCH"
    echo "  RTAI_HAL_PARAM     = $RTAI_HAL_PARAM"
    echo "  RTAI_SCHED_PARAM   = $RTAI_SCHED_PARAM"
}

function print_kernel_info {
    echo
    echo "loaded modules (lsmod):"
    if test -f lsmod.dat; then
	cat lsmod.dat | while read LINE; do echo "  $LINE"; done
	rm -f lsmod.dat
    else
	lsmod | while read LINE; do echo "  $LINE"; done
    fi
    echo
    print_distribution
    echo
    echo "hostname: $(hostname)"
    echo
    echo "running kernel (uname -r): $(uname -r)"
    echo
    echo "kernel parameter (/proc/cmdline):"
    for param in $(cat /proc/cmdline); do
	echo "  $param"
    done
    echo
    print_versions
    echo
    echo "CPU (/proc/cpuinfo):"
    echo "  $(grep "model name" /proc/cpuinfo | head -n 1)"
    echo "  number of cores: $CPU_NUM"
    echo "  machine (uname -m): $MACHINE"
    echo "  memory: $(free -h | grep Mem | awk '{print $2}') RAM"
    echo
    print_grub
    echo
    print_settings
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
	print_kernel_info
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
    PACKAGES="make gcc libncurses-dev zlib1g-dev g++ bc cvs git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev stress"
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
	    echo_log "$ ./${MAKE_RTAI_KERNEL} download rtai"
	    return 10
	fi
	cd ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/
	echo_log
	echo_log "Available patches for this machine ($RTAI_MACHINE), most latest last:"
	ls -rt -1 *.patch 2> /dev/null | tee -a "$LOG_FILE" | while read LINE; do echo "  $LINE"; done
	echo_log
	LINUX_KERNEL_V=${LINUX_KERNEL%.*}
	echo_log "Available patches for the selected kernel's kernel version ($LINUX_KERNEL_V):"
	ls -rt -1 *-${LINUX_KERNEL_V}*.patch 2> /dev/null | tee -a "$LOG_FILE" | while read LINE; do echo "  $LINE"; done
	echo_log
	echo_log "Available patches for the selected kernel ($LINUX_KERNEL):"
	ls -rt -1 *-${LINUX_KERNEL}*.patch 2> /dev/null | tee -a "$LOG_FILE" | while read LINE; do echo "  $LINE"; done
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
	echo_log "  $(uname -r)"
	echo_log
	echo_log "Choose a patch and set the RTAI_PATCH variable at the top of the script"
	echo_log "and the LINUX_KERNEL variable with the corresponding kernel version."
	echo_log
	echo_log "Suggested values:"
	echo_log
	echo_log "  RTAI_PATCH=\"${RTAI_PATCH}\""
	echo_log "  LINUX_KERNEL=\"${LINUX_KERNEL}\""
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
	    echo_log "  LINUX_KERNEL=\"${LINUX_KERNEL}\""
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
	echo_log "  remove it with $ ./${MAKE_RTAI_KERNEL} clean kernel"
    else
	if ! test -f linux-$LINUX_KERNEL.tar.xz; then
	    echo_log "archive linux-$LINUX_KERNEL.tar.xz not found."
	    echo_log "download it with $ ./${MAKE_RTAI_KERNEL} download kernel."
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
    cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
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

function prepare_kernel_configs {
    check_root
    echo "Prepare kernel configurations to be tested."
    echo
    echo "Step 1: save the original kernel configuration."
    KERNEL_CONFIG_FILE="kernelconfigs.mrk"
    if ! $DRYRUN; then
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	cp .config .config.origmrk
	cd - > /dev/null
	rm -f "$KERNEL_CONFIG_FILE"
    fi
    N=0
    echo
    echo "Step 2: modify and store the kernel configurations."
    read -p "  hit enter to continue ... "
    if ! $DRYRUN; then
	while true; do
	    cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    cp .config .config.mrk
	    make menuconfig
	    DESCRIPTION=""
	    if ! diff -q .config.mrk .config > /dev/null; then
		echo "  store patch of new configuration"
		diff -u .config.mrk .config > config.patch
		read -p "  short description of the kernel configuration (empty: finish) " DESCRIPTION
		echo
	    else
		break
	    fi
	    if test -z "$DESCRIPTION"; then
		break
	    else
		cd - > /dev/null
		let N+=1
		{
		    echo "#### START CONFIG $N $DESCRIPTION"
		    cat $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/config.patch
		    echo "#### END CONFIG $N $DESCRIPTION"
		    echo
		    echo
		} >> "$KERNEL_CONFIG_FILE"
	    fi
	done
	cd - > /dev/null
	# clean up:
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	rm -f .config.mrk config.patch
	mv .config.origmrk .config
	cd - > /dev/null
    fi
    echo
    echo "Step 3: saved $N kernel configuration(s) in file \""$KERNEL_CONFIG_FILE"\"."
    echo
    if test $N -gt 0; then
	echo "Step 4: go on and use the kernel configurations"
	echo "        by adding the following lines to a test batch file:"
	if test -f "$KERNEL_CONFIG_FILE"; then
	    sed -n -e '/^#### START CONFIG/{s/^#### START CONFIG \(.*\) \(.*\)/\2 : : CONFIG \1 '"$KERNEL_CONFIG_FILE"'/; p;}' "$KERNEL_CONFIG_FILE"
	fi
	echo
    fi
    exit 0
}

function build_kernel {
    cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
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
	    echo_log "keep already existing .configure file for linux-${KERNEL_NAME}."
	fi

	# build the kernel:
	echo_log "build the kernel"
	if ! $DRYRUN; then
	    export CONCURRENCY_LEVEL=$CPU_NUM
	    if $HAVE_MAKE_KPKG; then
		KM=""
		test -n "$KERNEL_MENU" && KM="--config $KERNEL_MENU"
		if $KERNEL_DEBUG; then
		    make-kpkg --initrd --append-to-version -${RTAI_DIR}${KERNEL_NUM} --revision 1.0 $KM kernel_image kernel_debug
		else
		    make-kpkg --initrd --append-to-version -${RTAI_DIR}${KERNEL_NUM} --revision 1.0 $KM kernel-image
		fi
	    else
		if test "$KERNEL_MENU" = "old"; then
		    make olddefconfig
		else
		    make $KERNEL_MENU
		fi
		make deb-pkg LOCALVERSION=-${RTAI_DIR}${KERNEL_NUM} KDEB_PKGVERSION=$(make kernelversion)-1
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
	echo_log "keep already compiled linux ${KERNEL_NAME} kernel."
    fi
}

function clean_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo_log "remove kernel sources $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
	if ! $DRYRUN; then
	    rm -r linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    rm linux
	fi
    fi
}

function install_kernel {
    cd "$KERNEL_PATH"
    KERNEL_PACKAGE=$(ls linux-image-${KERNEL_NAME}*.deb | tail -n 1)
    test -f "$KERNEL_PACKAGE" || KERNEL_PACKAGE=$(ls linux-image-${KERNEL_ALT_NAME}*.deb | tail -n 1)
    KERNEL_DEBUG_PACKAGE=$(ls linux-image-${KERNEL_NAME}-dbg_*.deb | tail -n 1)
    test -f "$KERNEL_DEBUG_PACKAGE" || KERNEL_DEBUG_PACKAGE=$(ls linux-image-${KERNEL_ALT_NAME}-dbg_*.deb | tail -n 1)
    if test -f "$KERNEL_PACKAGE"; then
	echo_log "install kernel from debian package $KERNEL_PACKAGE"
	if ! $DRYRUN; then
	    dpkg -i "$KERNEL_PACKAGE"
	    if $KERNEL_DEBUG; then
		dpkg -i "$KERNEL_DEBUG_PACKAGE"
	    fi
	    reboot_set_kernel
	fi
    else
	echo_log "no kernel to install"
	return 1
    fi
}

function uninstall_kernel {
    # kernel:
    if test ${CURRENT_KERNEL} = ${KERNEL_NAME} -o ${CURRENT_KERNEL} = ${KERNEL_ALT_NAME}; then
	echo_log "Cannot uninstall a running kernel!"
	echo_log "First boot into a different kernel. E.g. by executing"
	echo_log "$ ./${MAKE_RTAI_KERNEL} reboot"
	return 1
    fi
    echo_log "remove comedi kernel modules"
    if ! $DRYRUN; then
	rm -rf /lib/modules/${KERNEL_NAME}/comedi
	rm -rf /lib/modules/${KERNEL_ALT_NAME}/comedi
    fi
    echo_log "uninstall kernel ${KERNEL_NAME}"
    if ! $DRYRUN; then
	apt-get -y remove linux-image-${KERNEL_NAME}
	apt-get -y remove linux-image-${KERNEL_ALT_NAME}
    fi
}

function remove_kernel {
    cd $KERNEL_PATH
    if test -f linux-$LINUX_KERNEL.tar.xz; then
	echo_log "remove kernel package $KERNEL_PATH/linux-$LINUX_KERNEL.tar.xz"
	if ! $DRYRUN; then
	    rm linux-$LINUX_KERNEL.tar.xz
	fi
    fi
    KERNEL_PACKAGES=$(ls linux-image-${KERNEL_NAME}*.deb linux-image-${KERNEL_ALT_NAME}*.deb)
    echo_log "remove kernel package(s) " $KERNEL_PACKAGES
    if ! $DRYRUN; then
	rm $KERNEL_PACKAGES
    fi
}


###########################################################################
# reboot:

function setup_kernel_param {
    if test -f /etc/default/grub; then
	check_root
	if ! $DRYRUN; then
	    cd /etc/default
	    test -f grub.origkp && mv grub.origkp grub
	    cp grub grub.origkp
	    sed -e '/GRUB_CMDLINE_RTAI/s/=".*"/="'"$*"'"/' grub.origkp > grub
	    update-grub
	fi
	if test -n "$*"; then
	    echo_log ""
	    echo_log "Set RTAI kernel parameter to \"$*\"."
	fi
    else
	echo_log ""
	echo_log "/etc/default/grub not found: cannot configure kernel parameter"
    fi
}

function restore_kernel_param {
    if test -f /etc/default/grub.origkp; then
	echo_log "Restore original RTAI kernel parameter."
	if ! $DRYRUN; then
	    cd /etc/default
	    mv grub.origkp grub
	    update-grub
	fi
    fi
}

function reboot_set_kernel {
# tell grub to reboot into a specific kernel
# if no grub menu entry is specified boot into the rtai kernel
    if ! $DRYRUN; then
	GRUBMENU="$1"
	if test -z "$GRUBMENU"; then
	    GRUBMENU="$(grep '^\s*menuentry ' /boot/grub/grub.cfg | cut -d "'" -f 2 | grep "${LINUX_KERNEL}.*-${RTAI_DIR}${KERNEL_NUM}" | head -n 1)"
	fi
	grub-reboot "$GRUBMENU"
    fi
    echo_log "reboot into grub menu $GRUBMENU"
}

function reboot_cmd {
# reboot the computer
    echo_log "reboot now"
    if ! $DRYRUN; then
	if test "x$1" = "xroot"; then
	    echo_kmsg "REBOOT"
	    shutdown -r now
	    sleep 180
	    reboot -f
	    # reboot -f   # cold start
	    # reboot      # calls shutdown -r
	    # shutdown brings the system into the reuested runlevel (init 0/6)
	    # shutdown -r # reboot in a minute   
	    # shutdown -r now
	    # shutdown -r +<minutes>
	elif test "x$(id -u)" != "x0"; then
	    if qdbus --version &> /dev/null; then
		qdbus org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout 0 1 2
	    elif gnome-session-quit --version &> /dev/null; then
		gnome-session-quit --reboot --force
	    else
		shutdown -r now
	    fi
	else
	    echo_kmsg "REBOOT"
	    shutdown -r now
	fi
    fi
}

function reboot_kernel {
# N      : boot into Nth kernel
# keep   : boot into rtai kernel and keep previous set kernel parameter 
# default: boot into rtai kernel without additional kernel parameter
# ""     : boot into rtai kernel with additional kernel parameter as specified by KERNEL_PARAM
# XXX    : boot into rtai kernel with additional kernel parameter
# FILE   : boot into rtai kernel with kernel parameter taken from test results file FILE
    case $1 in
	[0-9]*)
	    reboot_set_kernel "$1"
	    sleep 2
	    reboot_cmd
	    ;;

	keep)
	    reboot_set_kernel
	    sleep 2
	    reboot_cmd
	    ;;

	default)
	    setup_kernel_param ""
	    reboot_set_kernel
	    sleep 2
	    reboot_cmd
	    ;;

	*)
	    if test -z "$*"; then
		setup_kernel_param $KERNEL_PARAM
	    elif test -f "$1"; then
		setup_kernel_param $(sed -n -e '/kernel parameter/,/Versions/p' "$1" | \
		    sed -e '1d; $d; s/^  //;' | \
		    sed -e '/BOOT/d; /^root/d; /^ro$/d; /^quiet/d; /^splash/d; /^vt.handoff/d;')
	    else
		setup_kernel_param $*
	    fi
	    reboot_set_kernel
	    sleep 2
	    reboot_cmd
	    ;;
    esac
}


###########################################################################
# tests:

function test_result {
    TESTMODE="$1"
    TEST_RESULT=""
    if test -n "$TESTMODE" && test -f "results-${TESTMODE}-latency.dat"; then
	TEST_DATA=$(grep RTD results-${TESTMODE}-latency.dat | tail -n 1)
	OVERRUNS=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{printf("%d", $7)}')
	if test "$OVERRUNS" -gt "0"; then
	    TEST_RESULT="failed"
	else
	    LAT_MIN=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{printf("%d", $3)}')
	    LAT_MAX=$(echo "$TEST_DATA" | awk -F '\\|[ ]*' '{printf("%d", $6)}')
	    LATENCY=$(( $LAT_MAX - $LAT_MIN ))
	    if test "$LATENCY" -gt 20000; then
		TEST_RESULT="bad"
	    elif test "$LATENCY" -gt 10000; then
		TEST_RESULT="ok"
	    elif test "$LATENCY" -gt 2000; then
		TEST_RESULT="good"
	    else
		TEST_RESULT="perfect"
	    fi
	fi
    else
	TEST_RESULT="missing"
    fi
    echo $TEST_RESULT
}

function test_save {
    NAME="$1"
    REPORT="$2"
    TESTED="$3"
    PROGRESS="$4"
    {
	# summary analysis of test results:
	echo "Test summary (in nanoseconds):"
	echo
	# header 1:
	printf "RTH| %-50s| " "general"
	for TD in kern kthreads user; do
	    printf "%-41s| %-19s| %-31s| " "$TD latencies" "$TD switches" "$TD preempt"
	done
	printf "%s\n" "kernel"
	# header 2:
	printf "RTH| %-40s| %-8s| " "description" "progress"
	for TD in kern kthreads user; do
	    printf "%7s| %7s| %7s| %5s| %7s| %5s| %5s| %5s| %9s| %9s| %9s| " "ovlmax" "avgmax" "std" "n" "maxover" "susp" "sem" "rpc" "max" "jitfast" "jitslow"
	done
	printf "%s\n" "configuration"
	# data:
	printf "RTD| %-40s| %-8s| " "$NAME" "$PROGRESS"
	for TD in kern kthreads user; do
	    T=${TD:0:1}
	    test "$TD" = "kthreads" && T="t"
	    TN=latency
	    TEST_RESULTS=results-$TD-$TN.dat
	    if test -f "$TEST_RESULTS"; then
		awk '{if ($1 == "RTD|") { d=$5-$2; sum+=d; sumsq+=d*d; n++; maxd=$6-$3; if (ors<$7) ors=$7}} END {if (n>0) printf( "%7.0f| %7.0f| %7.0f| %5d| %7d| ", maxd, sum/n, sqrt(sumsq/n-(sum/n)*(sum/n)), n, ors ) }' "$TEST_RESULTS"
	    elif [[ $TESTED == *${T}* ]]; then
		printf "%7s| %7s| %7s| %5s| %7s| " "o" "o" "o" "o" "o"
	    else
		printf "%7s| %7s| %7s| %5s| %7s| " "-" "-" "-" "-" "-"
	    fi
	    TN=switches
	    TEST_RESULTS=results-$TD-$TN.dat
	    if test -f "$TEST_RESULTS" && test "$(grep -c 'SWITCH TIME' "$TEST_RESULTS")" -eq 3; then
		grep 'SWITCH TIME' "$TEST_RESULTS" | awk '{ printf( "%5.0f| ", $(NF-1) ); }'
	    elif [[ $TESTED == *${T}* ]]; then
		printf "%5s| %5s| %5s| " "o" "o" "o"
	    else
		printf "%5s| %5s| %5s| " "-" "-" "-"
	    fi
	    TN=preempt
	    TEST_RESULTS=results-$TD-$TN.dat
	    if test -f "$TEST_RESULTS"; then
		awk '{if ($1 == "RTD|") { maxd=$4-$2; jfast=$5; jslow=$6; }} END { printf( "%9.0f| %9.0f| %9.0f| ", maxd, jfast, jslow ) }' "$TEST_RESULTS"
	    elif [[ $TESTED == *${T}* ]]; then
		printf "%9s| %9s| %9s| " "o" "o" "o"
	    else
		printf "%9s| %9s| %9s| " "-" "-" "-"
	    fi
	done
	printf "%s\n" "config-$REPORT"
	echo
	# failed modules:
	if [[ $TESTED == *h* ]] && [[ $PROGRESS != *h* ]]; then
	    echo "Failed to load rtai_hal module"
	    echo
	fi
	if [[ $TESTED == *s* ]] && [[ $PROGRESS != *s* ]]; then
	    echo "Failed to load rtai_sched module"
	    echo
	fi
	if [[ $TESTED == *m* ]] && [[ $PROGRESS != *m* ]]; then
	    echo "Failed to load rtai_math module"
	    echo
	fi
	if [[ $TESTED == *c* ]] && [[ $PROGRESS != *c* ]]; then
	    echo "Failed to load kcomedilib module"
	    echo
	fi
	# load processes:
	if test -f load.dat; then
	    echo "Load:"
	    sed -e 's/^[ ]*load[ ]*/  /' load.dat
	else
	    echo "Load: none"
	fi
	echo
	# original test results:
	for TD in kern kthreads user; do
	    for TN in latency switches preempt; do
		TEST_RESULTS=results-$TD-$TN.dat
		if test -f "$TEST_RESULTS"; then
		    echo "$TD/$TN test:"
		    sed -e '/^\*/d' $TEST_RESULTS
		    echo
		    echo
		fi
	    done
	done
	print_kernel_info
	echo
	echo "rtai-info reports:"
	${REALTIME_DIR}/bin/rtai-info
	echo
	echo "dmesg:"
	echo
	dmesg | tac | sed '/MAKERTAIKERNEL.SH.*START/q' | tac | sed -n '/MAKERTAIKERNEL.SH.*START/,/MAKERTAIKERNEL.SH.*DONE/p'
    } > latencies-$REPORT
    cp /boot/config-${KERNEL_NAME} config-$REPORT
    chown --reference=. latencies-$REPORT
    chown --reference=. config-$REPORT
}

function test_run {
    DIR=$1
    TEST=$2
    TEST_RESULTS=results-$DIR-$TEST.dat
    TEST_DIR=${REALTIME_DIR}/testsuite/$DIR/$TEST
    rm -f $TEST_RESULTS
    if test -d $TEST_DIR; then
	echo "running $DIR/$TEST test"
	echo_kmsg "RUN $DIR/$TEST test"
	cd $TEST_DIR
	rm -f $TEST_RESULTS

	# setup automatic test duration:
	TTIME=$TEST_TIME
	if test -n "$TEST_TIME" && test $TEST = switches; then
	    TTIME=2
	    test $DIR = user && TTIME=""
	fi
	if test -n "$TEST_TIME" && test $TEST = preempt && test $TEST_TIME -gt 10; then
	    TTIME=10
	fi
	TIMEOUTCMD=""
	if test -n "$TTIME"; then
	    TIMEOUTCMD="timeout -s SIGINT -k 1 $TTIME"
	fi

	# run the test:
	trap true SIGINT   # ^C should terminate ./run but not this script
	$TIMEOUTCMD ./run | tee $TEST_RESULTS
	#script -c "$TIMEOUTCMD ./run" results.dat
	trap - SIGINT
	#sed -e '1d; $d; s/\^C//' results.dat > $TEST_RESULTS
	#rm results.dat
	cd - > /dev/null
	mv $TEST_DIR/$TEST_RESULTS .
	echo
    fi
}

function test_kernel {
    check_root

    # check for kernel log messages:
    if ! test -f /var/log/messages; then
	echo_log "/var/log/messages does not exist!"
	echo_log "enable it by running:"
	echo_log "$ ./${MAKE_RTAI_KERNEL} setup messages"
	echo_log
	exit 1
    fi

    # test targets:
    TESTMODE=""
    LOADMODE=""
    MAXMODULE="4"
    CALIBRATE="false"
    DESCRIPTION=""
    TESTSPECS=""
    while test -n "$1"; do
	TESTSPECS="$TESTSPECS $1"
	case $1 in
	    hal) MAXMODULE="1" ;;
	    sched) MAXMODULE="2" ;;
	    math) MAXMODULE="3" ;;
	    comedi) MAXMODULE="4" ;;
	    kern) TESTMODE="$TESTMODE kern" ;;
	    kthreads) TESTMODE="$TESTMODE kthreads" ;;
	    user) TESTMODE="$TESTMODE user" ;;
	    all) TESTMODE="kern kthreads user" ;;
	    none) TESTMODE="none" ;;
	    calib) CALIBRATE="true" ;;
	    cpu) LOADMODE="$LOADMODE cpu" ;;
	    io) LOADMODE="$LOADMODE io" ;;
	    net) LOADMODE="$LOADMODE net" ;;
	    full) LOADMODE="cpu io mem net" ;;
	    [0-9]*) TEST_TIME="$((10#$1))" ;;
	    auto) shift; test -n "$1" && { DESCRIPTION="$1"; TESTSPECS="$TESTSPECS $1"; } ;;
	    batch) shift; test_batch "$1" "$TEST_TIME" "$TESTMODE" ${TESTSPECS% batch} ;;
	    batchscript) shift; test_batch_script ;;
	    *) echo "test $1 is invalid"
		exit 1 ;;
	esac
	shift
    done
    test -z "$TESTMODE" && TESTMODE="kern"
    TESTMODE=$(echo $TESTMODE)  # strip whitespace
    LOADMODE=$(echo $LOADMODE)  # strip whitespace

    if test ${CURRENT_KERNEL} != ${KERNEL_NAME} && test ${CURRENT_KERNEL} != ${KERNEL_ALT_NAME}; then
	echo "Need a running rtai kernel that matches the configuration of ${MAKE_RTAI_KERNEL}!"
	echo
	echo "Either boot into the ${KERNEL_NAME} kernel, e.g. by executing"
	echo "$ ./${MAKE_RTAI_KERNEL} reboot"
	echo "or supply the right parameter to ${MAKE_RTAI_KERNEL}."
	echo
	echo "Info:"
	echo "  Your running kernel is: ${CURRENT_KERNEL}"
	echo "  LINUX_KERNEL is set to ${LINUX_KERNEL}, set with -k"
	echo "  RTAI_DIR is set to ${RTAI_DIR}, set with -r"
	echo "  KERNEL_NUM is set to $KERNEL_NUM, set with -n"
	return 1
    fi

    if $DRYRUN; then
	echo "run some tests on currently running kernel ${KERNEL_NAME}"
	echo "  test mode(s)        : $TESTMODE"
	echo "  max module to load  : $MAXMODULE"
	echo "  rtai_sched parameter: $RTAI_SCHED_PARAM"
	echo "  rtai_hal parameter  : $RTAI_HAL_PARAM"
	echo "  description         : $DESCRIPTION"
	return 0
    fi

    # remove old test results:
    for TD in kern kthreads user; do
	for TN in latency switches preempt; do
	    TEST_RESULTS=results-$TD-$TN.dat
	    rm -f $TEST_RESULTS
	done
    done
    rm -f load.dat
    rm -f lsmod.dat

    # report number:
    REPORT_NAME=${LINUX_KERNEL}-${RTAI_DIR}-$(hostname)
    NUM=001
    LASTREPORT="$(ls latencies-${REPORT_NAME}-*-* 2> /dev/null | tail -n 1)"
    if test -n "$LASTREPORT"; then
	LASTREPORT="${LASTREPORT#latencies-${REPORT_NAME}-}"
	N="${LASTREPORT%%-*}"
	N=$(expr $N + 1)
	NUM="$(printf "%03d" $N)"
    fi

    if $CALIBRATE; then
	# remove latency file to force calibration:
	# this is for rtai5, for rtai4 the calibration tools needs to be run manually
	# see base/arch/x86/calibration/README
	if test -f ${REALTIME_DIR}/calibration/latencies; then
	    rm ${REALTIME_DIR}/calibration/latencies
	fi
    else
	# if not calibrated yet, provide default latencies:
	if ! test -f ${REALTIME_DIR}/calibration/latencies; then
	    RTAI_SCHED_PARAM="$RTAI_SCHED_PARAM kernel_latency=0 user_latency=0"
	fi
    fi

    # description of kernel configuration:
    if test -z "$DESCRIPTION"; then
	read -p 'Please enter a short name describing the kernel configuration (empty: abort tests): ' NAME
	test -z "$NAME" && return
    else
	NAME="$DESCRIPTION"
    fi
    # add load information to description:
    if test -n "$LOADMODE"; then
	NAME="${NAME}-"
	for LOAD in $LOADMODE; do
	    NAME="${NAME}${LOAD:0:1}"
	done
    fi
    REPORT_NAME="${REPORT_NAME}-${NUM}-$(date '+%F')-${NAME}"
    REPORT="${REPORT_NAME}-failed"

    # unload already loaded comedi kernel modules:
    remove_comedi_modules
    # unload already loaded rtai kernel modules:
    for MOD in msg mbx sem math sched hal; do
	lsmod | grep -q rtai_$MOD && { rmmod rtai_$MOD && echo "removed already loaded rtai_$MOD"; }
    done
    echo

    TESTED=""
    PROGRESS=""
    echo_kmsg "START TESTS"

    # loading rtai kernel modules:
    RTAIMOD_FAILED=false

    # rtai_hal:
    if test $MAXMODULE -ge 1; then
	TESTED="${TESTED}h"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	echo_kmsg "INSMOD ${REALTIME_DIR}/modules/rtai_hal.ko $RTAI_HAL_PARAM"
	lsmod | grep -q rtai_hal || { insmod ${REALTIME_DIR}/modules/rtai_hal.ko $RTAI_HAL_PARAM && echo "loaded  rtai_hal $RTAI_HAL_PARAM" || RTAIMOD_FAILED=true; }
	$RTAIMOD_FAILED || PROGRESS="${PROGRESS}h"
    fi

    # rtai_sched:
    if test $MAXMODULE -ge 2; then
	TESTED="${TESTED}s"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	echo_kmsg "INSMOD ${REALTIME_DIR}/modules/rtai_sched.ko $RTAI_SCHED_PARAM"
	lsmod | grep -q rtai_sched || { insmod ${REALTIME_DIR}/modules/rtai_sched.ko $RTAI_SCHED_PARAM && echo "loaded  rtai_sched $RTAI_SCHED_PARAM" || RTAIMOD_FAILED=true; }
	$RTAIMOD_FAILED || PROGRESS="${PROGRESS}s"
    fi

    # rtai_math:
    if test $MAXMODULE -ge 3; then
	if test -f ${REALTIME_DIR}/modules/rtai_math.ko; then
	    TESTED="${TESTED}m"
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	    echo_kmsg "INSMOD ${REALTIME_DIR}/modules/rtai_math.ko"
	    lsmod | grep -q rtai_math || { insmod ${REALTIME_DIR}/modules/rtai_math.ko && echo "loaded  rtai_math" && PROGRESS="${PROGRESS}m"; }
	else
	    echo "rtai_math is not available"
	fi
    fi
    
    if test $MAXMODULE -ge 4 && $MAKE_COMEDI && ! $RTAIMOD_FAILED; then
	TESTED="${TESTED}c"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	# loading comedi:
	echo_kmsg "LOAD COMEDI MODULES"
	echo "triggering comedi "
	udevadm trigger
	sleep 1
	modprobe kcomedilib && echo "loaded  kcomedilib"

	lsmod | grep -q kcomedilib && PROGRESS="${PROGRESS}c"
	
	lsmod > lsmod.dat
	
	echo_kmsg "REMOVE COMEDI MODULES"
	remove_comedi_modules
    fi
    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
    
    # remove rtai modules:
    if test $MAXMODULE -ge 3; then
	echo_kmsg "RMMOD rtai_math"
	lsmod | grep -q rtai_math && { rmmod rtai_math && echo "removed rtai_math"; }
    fi
    if test $MAXMODULE -ge 2; then
	echo_kmsg "RMMOD rtai_sched"
	lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo "removed rtai_sched"; }
    fi
    if test $MAXMODULE -ge 1; then
	echo_kmsg "RMMOD rtai_hal"
	lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo "removed rtai_hal"; }
    fi

    # loading modules failed:
    if $RTAIMOD_FAILED; then
	echo "Failed to load RTAI modules."
	echo
	if test -z "$DESCRIPTION"; then
	    read -p 'Save configuration? (y/N): ' SAVE
	    if test "$SAVE" = "y"; then
		echo
		echo "saved kernel configuration in: config-$REPORT"
		echo "saved test results in        : latencies-$REPORT"
	    else
		rm -f config-$REPORT
		rm -f latencies-$REPORT
	    fi
	fi
	return
    fi
    echo "successfully loaded and unloaded rtai modules"
    echo

    # RTAI tests:
    if test "$TESTMODE" != none; then
	# stress program available?
	STRESS=false
	stress --version &> /dev/null && STRESS=true
	# produce load:
	LOAD_PIDS=()
	LOAD_FILES=()
	test -n "$LOADMODE" && echo "start some jobs to produce load:"
	for LOAD in $LOADMODE; do
	    case $LOAD in
		cpu) if $STRESS; then
	                echo "  load cpu: stress -c $CPU_NUM" | tee -a load.dat
			stress -c $CPU_NUM &> /dev/null
                    else
	                echo "  load cpu: seq $CPU_NUM | xargs -P0 -n1 md5sum /dev/urandom" | tee -a load.dat
			seq $CPU_NUM | xargs -P0 -n1 md5sum /dev/urandom 
                    fi
                    LOAD_PIDS+=( $! )
		    ;;
		io) if $STRESS; then
	                echo "  load io: stress --hdd-bytes 128M -d $CPU_NUM" | tee -a load.dat
			stress --hdd-bytes 128M -d $CPU_NUM &> /dev/null
		    else
		        echo "  load io: ls -lR" | tee -a load.dat
			while true; do ls -lR / &> load-lsr; done & 
			LOAD_PIDS+=( $! )
			LOAD_FILES+=( load-lsr )
			echo "  load io: find" | tee -a load.dat
			while true; do find / -name '*.so' &> load-find; done & 
			LOAD_PIDS+=( $! )
			LOAD_FILES+=( load-find )
		    fi
		    ;;
		mem) if $STRESS; then
	                echo "  load mem: stress -m $CPU_NUM" | tee -a load.dat
			stress -m $CPU_NUM &> /dev/null
		    else
		        echo "  load mem: no test available"
		    fi
		    ;;
		net) echo "  load net: ping -f localhost" | tee -a load.dat
		    ping -f localhost > /dev/null &
		    LOAD_PIDS+=( $! )
		    if test -n "$REMOTE_MACHINE"; then
			echo "  load net: ping -f $REMOTE_MACHINE" | tee -a load.dat
			ping -f $REMOTE_MACHINE > /dev/null & 
			LOAD_PIDS+=( $! )
		    fi 
		    ;;
		snd) echo "  load snd: not implemented yet!" ;;
	    esac
	done
	test -n "$LOADMODE" && echo

	# run tests:
	for DIR in $TESTMODE; do
	    TT=${DIR:0:1}
	    test "$DIR" = "kthreads" && TT="t"
	    TESTED="${TESTED}${TT}"
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"

	    test_run $DIR latency
	    if test $DIR = ${TESTMODE%% *}; then
		rm -f config-$REPORT
		rm -f latencies-$REPORT
		TEST_RESULT="$(test_result ${TESTMODE%% *})"
		REPORT="${REPORT_NAME}-${TEST_RESULT}"
	    fi
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"

	    test_run $DIR switches
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"

	    test_run $DIR preempt
	    PROGRESS="${PROGRESS}${TT}"
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	done
	echo_kmsg "TESTS DONE"
	echo "finished all tests"
	echo
    fi

    # clean up load:
    for PID in ${LOAD_PIDS[@]}; do
	kill -KILL $PID
    done
    for FILE in ${LOAD_FILES[@]}; do
	rm -f $FILE
    done
    # clean up:
    for MOD in msg mbx sem math sched hal; do
	lsmod | grep -q rtai_$MOD && { rmmod rtai_$MOD && echo "removed loaded rtai_$MOD"; }
    done
    
    # report:
    rm -f config-$REPORT
    rm -f latencies-$REPORT
    TEST_RESULT="$(test_result ${TESTMODE%% *})"
    if test -z "$DESCRIPTION"; then
	read -p "Please enter a short description of the test result (empty: $TEST_RESULT, n: don't save): " RESULT
	test -z "$RESULT" && RESULT="$TEST_RESULT"
    else
	RESULT="$TEST_RESULT"
    fi
    if test "$RESULT" != n; then
	REPORT="${REPORT_NAME}-${RESULT}"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	echo
	echo "saved kernel configuration in : config-$REPORT"
	echo "saved test results in         : latencies-$REPORT"
    else
	echo "test results not saved"
    fi

    # remove test results:
    for TD in kern kthreads user; do
	for TN in latency switches preempt; do
	    TEST_RESULTS=results-$TD-$TN.dat
	    rm -f $TEST_RESULTS
	done
    done
    rm -f load.dat
    rm -f lsmod.dat
}

function test_batch {
    # setup automatic testing of kernel parameter

    BATCH_FILE="$1"
    if test -z "$BATCH_FILE"; then
	echo "You need the specify a file that lists the kernel parameter to be tested:"
	echo "$ ./${MAKE_RTAI_KERNEL} batch FILE"
	exit 1
    fi
    if ! test -f "$BATCH_FILE"; then
	if test "$BATCH_FILE" = "basic"; then
	    BATCH_FILE=testbasic.mrk
	    if test -f $BATCH_FILE; then
		echo "File \"$BATCH_FILE\" already exists."
		echo "Cannot write basic kernel parameter."
		exit 1
	    fi
	    cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# batch file for testing RTAI kernel with various kernel parameter.
#
# Each line has the format:
# <description> : <load specification> : <kernel parameter>
# where <description> is a brief one-word description of the kernel
# configuration and the kernel parameter that is added to the
# KERNEL_PARAM_DESCR variable.  The <kernel parameter> are added to
# the ones defined in the KERNEL_PARAM variable.
#
# Edit this file according to your needs.
#
# Then run
#
# $ ./$MAKE_RTAI_KERNEL test math 121 batch $BATCH_FILE
#
# for testing all the kernel parameter. Have a cup of tea and come back
# (each configuration needs about 5 minutes for booting, testing and shutdown).
# The test results are recorded in the latencies-${LINUX_KERNEL}-${RTAI_DIR}-$(hostname)* files.
# Generate a summary table by calling
#
# $ ./$MAKE_RTAI_KERNEL report | less -S

# test two times to see variability of results:
plain1 : :
plain2 : :

# clocks and timers:
nohz : : nohz=off
tscreliable : : tsc=reliable
tscnoirqtime : : tsc=noirqtime
nolapictimer : : nolapic_timer  # no good
clocksourcehpet : : clocksource=hpet
highresoff : : highres=off
hpetdisable : : hpet=disable
skewtick : : skew_tick=1

# test yet again to see variability of results:
plain3 : :
EOF
	    chown --reference=. $BATCH_FILE
	    echo "Wrote default kernel parameter to be tested into file \"$BATCH_FILE\"."
	    echo ""
	    echo "Call test batch again with something like"
	    echo "$ ./${MAKE_RTAI_KERNEL} test 90 batch $BATCH_FILE"
	    exit 0
	elif test "$BATCH_FILE" = "acpi" || test "$BATCH_FILE" = "apic"; then
	    BATCH_FILE=testacpi.mrk
	    if test -f $BATCH_FILE; then
		echo "File \"$BATCH_FILE\" already exists."
		echo "Cannot write basic kernel parameter."
		exit 1
	    fi
	    cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# batch file for testing RTAI kernel with various kernel parameter.

plain : :

# acpi:
#acpioff : : acpi=off    # often very effective, but weired system behavior
acpinoirq : : acpi=noirq
pcinoacpi : : pci=noacpi
pcinomsi : : pci=nomsi

# apic:
noapic : : noapic
nox2apic : : nox2apic
x2apicphys : : x2apic_phys
#nolapic : : nolapic    # we need the lapic timer!
lapicnotscdeadl : : lapic=notscdeadline
EOF
	    chown --reference=. $BATCH_FILE
	    echo "Wrote default kernel parameter to be tested into file \"$BATCH_FILE\"."
	    echo ""
	    echo "Call test batch again with something like"
	    echo "$ ./${MAKE_RTAI_KERNEL} test 90 batch $BATCH_FILE"
	    exit 0
	elif test "$BATCH_FILE" = "isolcpus"; then
	    BATCH_FILE=testisolcpus.mrk
	    if test -f $BATCH_FILE; then
		echo "File \"$BATCH_FILE\" already exists."
		echo "Cannot write isolcpus kernel parameter."
		exit 1
	    fi
	    cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# batch file for testing RTAI kernel with cpu isolation

plain : :
plain : full :

# isolcpus:
isolcpus : : isolcpus=0-1
isolcpus : full : isolcpus=0-1
isolcpus-nohzfull : full : isolcpus=0-1 nohz_full=0-1
isolcpus-rcu : full : isolcpus=0-1 rcu_nocbs=0-1
isolcpus-nohzfull-rcu : full : isolcpus=0-1 nohz_full=0-1 rcu_nocbs=0-1

# devices:
dma : : libata.dma=0
dma : full : libata.dma=0
EOF
	    chown --reference=. $BATCH_FILE
	    echo "Wrote default kernel parameter to be tested into file \"$BATCH_FILE\"."
	    echo ""
	    echo "Call test batch again with something like"
	    echo "$ ./${MAKE_RTAI_KERNEL} test 90 batch $BATCH_FILE"
	    exit 0
	fi
	echo "File \"$BATCH_FILE\" does not exist!"
	exit 1
    fi
    N_TESTS=$(sed -e 's/ *#.*$//' $BATCH_FILE | grep -c ':.*:')
    if test $N_TESTS -eq 0; then
	echo "No valid configurations specified in file \"$BATCH_FILE\"!"
	exit 1
    fi

    shift
    TEST_TIME="$((10#$1))"
    shift
    TESTMODE="$1"
    test -z "$TESTMODE" && TESTMODE="kern"
    TESTMODE=$(echo $TESTMODE)  # strip whitespace
    shift
    TEST_SPECS="$@"

    # compute total time needed for the tests:
    if test -z "$TEST_TIME"; then
	TEST_TIME="600"
	TEST_SPECS="$TEST_SPECS $TEST_TIME"
    fi
    TEST_TOTAL_TIME=5
    for TM in $TESTMODE; do
	let TEST_TOTAL_TIME+=$TEST_TIME
	let TEST_TOTAL_TIME+=20
    done

    echo "run \"test $TEST_SPECS\" on batch file \"$BATCH_FILE\" with content:"
    sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | while read LINE; do echo "  $LINE"; done
    echo


    # read first line from configuration file:
    INDEX=1
    IFS=':' read DESCRIPTION LOAD_MODE NEW_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
    KPD="$KERNEL_PARAM_DESCR"
    if test -n "$KPD" && test "${KPD:-1:1}" != "-"; then
	KPD="${KPD}-"
    fi
    echo "Reboot into first configuration: \"${BD}$(echo $DESCRIPTION)\" with kernel parameter \"$(echo $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEW_KERNEL_PARAM)\""
    echo

    # confirm:
    read -p "Do you want to proceed testing with $N_TESTS reboots (Y/n)? " PROCEED
    if test "x$PROCEED" != "xn"; then
	echo

	restore_test_batch
	# install crontab:
	MRK_DIR="$(cd "$(dirname "$0")" && pwd)"
	(crontab -l 2>/dev/null; echo "@reboot ${MRK_DIR}/${MAKE_RTAI_KERNEL} test batchscript > ${TEST_DIR}/testbatch.log") | crontab -
	echo "Installed crontab for automatic testing after reboot."
	echo "  Uninstall by calling"
	echo "  $ ./${MAKE_RTAI_KERNEL} restore testbatch"

	TEST_DIR="$(cd "$(dirname "$BATCH_FILE")" && pwd)"
	echo_kmsg "NEXT TEST BATCH |$TEST_DIR/${BATCH_FILE##*/}|$INDEX|$KERNEL_PARAM_DESCR|$TEST_TOTAL_TIME|$TEST_SPECS"

	reboot_kernel $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEW_KERNEL_PARAM
    else
	echo
	echo "Test batch aborted"
    fi

    exit 0
}

function test_batch_script {
    # run automatic testing of kernel parameter.
    # this function is called automatically after reboot from cron.

    PATH="$PATH:/usr/sbin:/usr/bin:/sbin:/bin"

    # wait:
    sleep $STARTUP_TIME

    # get test BATCH_FILE, INDEX, BATCH_DESCRIPTION, TEST_TOTAL_TIME and TEST_SPECS from /var/log/messages:
    IFS='|' read ID BATCH_FILE INDEX BATCH_DESCRIPTION TEST_TOTAL_TIME TEST_SPECS < <(grep "NEXT TEST BATCH" /var/log/messages | tail -n 1)

    N_TESTS=$(sed -e 's/ *#.*$//' $BATCH_FILE | grep -c ':.*:')

    # read current DESCRIPTION and LOAD_MODE from configuration file:
    IFS=':' read DESCRIPTION LOAD_MODE NEW_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
    # compile new kernel:
    COMPILE=false
    test "x${NEW_KERNEL_PARAM:0:6}" != "xCONFIG" && COMPILE=true
    # next:
    let INDEX+=1


    if test "$INDEX" -gt "$N_TESTS"; then
	# no further tests:
	echo_kmsg "LAST TEST BATCH"
	# clean up:
	restore_test_batch > /dev/null
	restore_kernel_param
	if ! $COMPILE; then
	    # at TEST_TOTAL_TIME seconds later reboot into default kernel:
	    { sleep $TEST_TOTAL_TIME; reboot_cmd root; } &
	fi
    else
	# read and set next NEXT_KERNEL_PARAM:
	IFS=':' read DESCR LM NEXT_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
	echo_kmsg "NEXT TEST BATCH |$BATCH_FILE|$INDEX|$BATCH_DESCRIPTION|$TEST_TOTAL_TIME|$TEST_SPECS"
	if test "x${NEXT_KERNEL_PARAM:0:6}" != "xCONFIG"; then
	    setup_kernel_param $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEXT_KERNEL_PARAM
	    reboot_set_kernel
	fi
	if ! $COMPILE; then
	    # at TEST_TOTAL_TIME seconds later reboot:
	    { sleep $TEST_TOTAL_TIME; reboot_cmd root; } &
	fi
    fi

    if $COMPILE; then
	# compile new kernel:
	echo_kmsg "START COMPILE NEW KERNEL"
	CONFIG_NUM="$(echo $NEW_KERNEL_PARAM | cut -d ' ' -f 2)"
	CONFIG_FILE="$(echo $NEW_KERNEL_PARAM | cut -d ' ' -f 3)"
	sed -n -e "/^#### START CONFIG $CONFIG_NUM/,/^#### END CONFIG $CONFIG_NUM/p" $CONFIG_FILE | sed -e '1d; $d;' | patch $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/.config
	KERNEL_MENU=old
	reconfigure
	echo_kmsg "END COMPILE NEW KERNEL"
    else
	# run tests:
	if test -n "$BATCH_DESCRIPTION" && test "${BATCH_DESCRIPTION:-1:1}" != "-"; then
	    BATCH_DESCRIPTION="${BATCH_DESCRIPTION}-"
	fi
	cd $(dirname $BATCH_FILE)
	test_kernel $TEST_SPECS $LOAD_MODE auto "$(echo $BATCH_DESCRIPTION)$(echo $DESCRIPTION)"

	if test "$INDEX" -gt "$N_TESTS"; then
	    echo_kmsg "FINISHED TEST BATCH"
	fi
    fi

    # reboot:
    sleep 1
    reboot_cmd root

    exit 0
}

function restore_test_batch {
    if crontab -l | grep -q "${MAKE_RTAI_KERNEL}"; then
	echo_log "restore original crontab"
	if ! $DRYRUN; then
	    (crontab -l | grep -v "${MAKE_RTAI_KERNEL}") | crontab -
	fi
    fi
}

function test_report {
    SORT=false
    SORTCOL=5
    if test "x$1" = "xavg"; then
	SORT=true
	SORTCOL=5
	shift
    elif test "x$1" = "xmax"; then
	SORT=true
	SORTCOL=4
	shift
    fi
    FILES="latencies-${LINUX_KERNEL}-${RTAI_DIR}-*"
    test -n "$1" && FILES="$*"
    test -d "$FILES" && FILES="$FILES/latencies-${LINUX_KERNEL}-${RTAI_DIR}-*"
    rm -f header.txt data.txt dataoverrun.txt
    # column widths:
    COLWS=()
    INIT=true
    for TEST in $FILES; do
	test -f "$TEST" || continue
	LINEMARKS="RTD|"
	$INIT && LINEMARKS="$LINEMARKS RTH|"
	for LINEMARK in $LINEMARKS; do
	    ORGIFS="$IFS"
	    IFS=" | "
	    INDEX=0
	    for COL in $(head -n 6 $TEST | fgrep "$LINEMARK" | tail -n 1); do
		# strip spaces:
		C=$(echo $COL)
		WIDTH=${#C}
		#if test "x$C" = "x-" || test "x$C" = "xo"; then
		#    WIDTH=0
		#fi
		if $INIT; then
		    COLWS+=($WIDTH)
		else
		    test "${COLWS[$INDEX]}" -lt "$WIDTH" && COLWS[$INDEX]=$WIDTH
		fi
		let INDEX+=1
	    done
	    IFS="$ORGIFS"
	    INIT=false
	done
    done
    # nothing found:
    if test ${#COLWS[*]} -le 2; then
	echo "You need to specify exisitng files or a directory with test result files!"
	echo
	echo "Usage:"
	echo "${MAKE_RTAI_KERNEL} report <FILES>"
	exit 1
    fi
    # column width for first line of header:
    INDEX=0
    HCOLWS=()
    WIDTH=0; for IDX in $(seq 1); do let WIDTH+=${COLWS[$INDEX]}; let INDEX+=1; done
    HCOLWS+=($WIDTH)
    WIDTH=2; for IDX in $(seq 2); do let WIDTH+=${COLWS[$INDEX]}; let INDEX+=1; done
    HCOLWS+=($WIDTH)
    for TD in $(seq 3); do
	WIDTH=8; for IDX in $(seq 5); do let WIDTH+=${COLWS[$INDEX]}; let INDEX+=1; done
	HCOLWS+=($WIDTH)
	WIDTH=4; for IDX in $(seq 3); do let WIDTH+=${COLWS[$INDEX]}; let INDEX+=1; done
	HCOLWS+=($WIDTH)
	WIDTH=4; for IDX in $(seq 3); do let WIDTH+=${COLWS[$INDEX]}; let INDEX+=1; done
	HCOLWS+=($WIDTH)
    done
    HCOLWS+=(${COLWS[$INDEX]})
    # reformat output to the calculated column widths:
    FIRST=true
    for TEST in $FILES; do
	test -f "$TEST" || continue
	LINEMARKS="RTD|"
	$FIRST && LINEMARKS="RTH| $LINEMARKS"
	FIRST=false
	for LINEMARK in $LINEMARKS; do
	    DEST=""
	    if test $LINEMARK = "RTH|"; then
		DEST="header.txt"
		# first line of header:
		ORGIFS="$IFS"
		IFS="|"
		MAXINDEX=${#HCOLWS[*]}
		let MAXINDEX-=1
		INDEX=0
		for COL in $(head -n 6 $TEST | fgrep "$LINEMARK" | head -n 1); do
		    IFS=" "
		    C=$(echo $COL)
		    IFS="|"
		    if test $INDEX -ge $MAXINDEX; then
			printf "%s\n" $C >> $DEST
		    else
			printf "%-${HCOLWS[$INDEX]}s| " ${C:0:${HCOLWS[$INDEX]}} >> $DEST
		    fi
		    let INDEX+=1
		done
		IFS="$ORGIFS"
	    else
		if ! $SORT; then
		    DEST="data.txt"
		elif test $(echo $(head -n 6 $TEST | fgrep 'RTD|' | cut -d '|' -f 8)) = "-"; then
		    DEST="dataoverrun.txt"
		elif test $(echo $(head -n 6 $TEST | fgrep 'RTD|' | cut -d '|' -f 8)) = "o"; then
		    DEST="dataoverrun.txt"
		elif test $(head -n 6 $TEST | fgrep 'RTD|' | cut -d '|' -f 8) -gt 0; then
		    DEST="dataoverrun.txt"
		else
		    DEST="data.txt"
		fi
	    fi
	    ORGIFS="$IFS"
	    IFS=" | "
	    INDEX=0
	    MAXINDEX=${#COLWS[*]}
	    let MAXINDEX-=1
	    for COL in $(head -n 6 $TEST | fgrep "$LINEMARK" | tail -n 1); do
		if test $INDEX -ge $MAXINDEX; then
		    printf "%s\n" $COL >> $DEST
		elif test $INDEX -lt 3; then
		    printf "%-${COLWS[$INDEX]}s| " $COL >> $DEST
		else
		    printf "%${COLWS[$INDEX]}s| " $COL >> $DEST
		fi
		let INDEX+=1
	    done
	    IFS="$ORGIFS"
	done
    done
    # sort results with respect to average kern latency:
    if ! $FIRST; then
	cat header.txt
	if $SORT; then
	    test -f data.txt && sort -t '|' -k $SORTCOL -n data.txt
	    test -f dataoverrun.txt && sort -t '|' -k $SORTCOL -n dataoverrun.txt
	else
	    test -f data.txt && cat data.txt
	fi
    fi
    rm -f header.txt data.txt dataoverrun.txt
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

function build_newlib {
    cd ${LOCAL_SRC_PATH}/newlib/install
    LIBM_PATH=$(find ${LOCAL_SRC_PATH}/newlib/install/ -name 'libm.a' | head -n 1)
    if test -f "$LIBM_PATH"; then
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
    if $NEW_KERNEL || $NEW_NEWLIB || ! test -f base/sched/rtai_sched.ko || ! test -f ${REALTIME_DIR}/modules/rtai_hal.ko; then
	echo_log "build rtai"
	if ! $DRYRUN; then
	    # path to newlib math library:
	    LIBM_PATH=$(find ${LOCAL_SRC_PATH}/newlib/install/ -name 'libm.a' | head -n 1)
	    test -z "$LIBM_PATH" && MAKE_NEWLIB=false
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
@@ -7,8 +7,8 @@
 #
 # General
 #
-CONFIG_RTAI_INSTALLDIR="/usr/realtime"
-CONFIG_RTAI_LINUXDIR="/usr/src/linux"
+CONFIG_RTAI_INSTALLDIR="${REALTIME_DIR}"
+CONFIG_RTAI_LINUXDIR="$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
 
 #
 # RTAI Documentation
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
	echo_log "keep already built and installed rtai modules"
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
    if test -d ${REALTIME_DIR}; then
	echo_log "uninstall rtai"
	if ! $DRYRUN; then
	    rm -r ${REALTIME_DIR}
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
	    PATH="$PATH:${REALTIME_DIR}/bin"
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
	PATH="$PATH:${REALTIME_DIR}/bin"
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
	    PATH="$PATH:${REALTIME_DIR}/bin"
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

function build_comedi {
    BUILT_COMEDI=false
    $NEW_RTAI && BUILT_COMEDI=true
    ! test -f ${LOCAL_SRC_PATH}/comedi/comedi/comedi.o && BUILT_COMEDI=true
    test -f /usr/local/src/comedi/config.status && ! grep -q modules/${KERNEL_NAME} /usr/local/src/comedi/config.status && BUILT_COMEDI=true
    if $BUILT_COMEDI; then
	cd ${LOCAL_SRC_PATH}/comedi
	echo_log "build comedi"
	if ! $DRYRUN; then
	    ./autogen.sh
	    PATH="$PATH:${REALTIME_DIR}/bin"
	    ./configure --with-linuxdir=$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} --with-rtaidir=${REALTIME_DIR}
	    make clean
	    cp ${REALTIME_DIR}/modules/Module.symvers comedi/
	    make -j $CPU_NUM
	    if test "x$?" != "x0"; then
		echo_log "Failed to build comedi!"
		return 1
	    fi
	    install_comedi || return 1
	fi
	NEW_COMEDI=true
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

function install_comedi {
    echo_log "remove all loaded comedi kernel modules"
    remove_comedi_modules

    echo_log "remove comedi staging kernel modules"
    if ! $DRYRUN; then
	rm -rf /lib/modules/${KERNEL_NAME}/kernel/drivers/staging/comedi
	rm -rf /lib/modules/${KERNEL_ALT_NAME}/kernel/drivers/staging/comedi
    fi

    cd ${LOCAL_SRC_PATH}/comedi
    echo_log "install comedi"
    if ! $DRYRUN; then
	make install
	if test "x$?" != "x0"; then
	    echo_log "Failed to install comedi!"
	    return 1
	fi
	KERNEL_MODULES=/lib/modules/${KERNEL_NAME}
	test -d "$KERNEL_MODULES" || KERNEL_MODULES=/lib/modules/${KERNEL_ALT_NAME}
	cp ${LOCAL_SRC_PATH}/comedi/comedi/Module.symvers ${KERNEL_MODULES}/comedi/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedi.h /usr/include/linux/
	cp ${LOCAL_SRC_PATH}/comedi/include/linux/comedilib.h /usr/include/linux/
	depmod -a
	sleep 1
	udevadm trigger
    fi
    NEW_COMEDI=true
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

function remove_comedi_modules {
    if lsmod | grep -q kcomedilib; then
	modprobe -r kcomedilib && echo "removed kcomedilib"
	for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	    modprobe -r $i && echo "removed $i"
	done
	modprobe -r comedi && echo "removed comedi"
    fi
}


###########################################################################
# /var/log/messages:

function setup_messages {
    if test -f /etc/rsyslog.d/50-default.conf.origmrk; then
	echo_log "/etc/rsyslog.d/50-default.conf has already been modified to enable /var/log/messages"
    elif ! test -f /var/log/messages || test /var/log/messages -ot /var/log/$(ls -rt /var/log | tail -n 1); then
	cd /etc/rsyslog.d
	if test -f 50-default.conf; then
	    echo_log "Patch /etc/rsyslog.d/50-default.conf to enable /var/log/messages"
	    if ! $DRYRUN; then
		cp 50-default.conf 50-default.conf.origmrk
		sed -e '/info.*notice.*warn/,/messages/s/#//' 50-default.conf.origmrk > 50-default.conf
		service rsyslog restart
		sleep 1
		test -f /var/log/messages || echo_log "failed to enable /var/log/messages"
	    fi
	else
	    echo_log "/etc/rsyslog.d/50-default.conf not found: cannot enable /var/log/messages"
	fi
    fi
}

function restore_messages {
    cd /etc/rsyslog.d
    if test -f 50-default.conf.origmrk; then
	echo_log "Restore original /etc/rsyslog.d/50-default.conf"
	if ! $DRYRUN; then
	    mv 50-default.conf.origmrk 50-default.conf
	    service rsyslog restart
	fi
    fi
}


###########################################################################
# grub menu:

function setup_grub {
    RUN_UPDATE=false
    if test -f /etc/default/grub.origmrk; then
	echo_log "Grub menu has already been configured."
    elif test -f /etc/default/grub; then
	cd /etc/default
	echo_log "Configure grub menu."
	if ! $DRYRUN; then
	    cp grub grub.origmrk
	    sed -e "s/GRUB_HIDDEN/#GRUB_HIDDEN/; s/GRUB_TIMEOUT=.*/GRUB_TIMEOUT=5/; /GRUB_CMDLINE_LINUX=/aexport GRUB_CMDLINE_RTAI=\"$KERNEL_PARAM\"" grub.origmrk > grub
	    ( echo; echo "GRUB_DISABLE_SUBMENU=y"; echo; echo "GRUB_DISABLE_RECOVERY=true" ) >> grub
	    RUN_UPDATE=true
	fi
    else
	echo_log "/etc/default/grub not found: cannot configure grub menu."
    fi
    if test -f /etc/grub.d/10_linux.origmrk; then
	echo_log "Grub linux script has already been configured."
    elif test -f /etc/grub.d/10_linux; then
	cd /etc/grub.d
	echo_log "Configure grub linux entries."
	if ! $DRYRUN; then
	    mv 10_linux 10_linux.origmrk
	    sed -e '/SUPPORTED_INITS/{s/ systemd.*systemd//; s/ upstart.*upstart//;}' -e '/\${GRUB_CMDLINE_LINUX}.*\${GRUB_CMDLINE_LINUX_DEFAULT}/s/\(${GRUB_CMDLINE_LINUX}.*\)\(${GRUB_CMDLINE_LINUX_DEFAULT}\)/\1${CMDLINE_RTAI} \2/' -e '/initramfs=$/i # check for RTAI kernel:\
  CMDLINE_RTAI=""\
  if grep -q "CONFIG_IPIPE=y" "${config}"; then\
    CMDLINE_RTAI="${GRUB_CMDLINE_RTAI}"\
  fi' 10_linux.origmrk > 10_linux
	    chmod a-x 10_linux.origmrk
	    chmod a+x 10_linux
	    RUN_UPDATE=true
	fi
    else
	echo_log "/etc/grub.d/10_linux not found: cannot configure grub linux entries."
    fi
    if test -f /boot/grub/grubenv; then
	echo_log "Enable reboot requests for normal user."
	if ! $DRYRUN; then
	    chmod a+w /boot/grub/grubenv
	fi
    else
	echo_log "/boot/grub/grubenv not found: cannot enable reboot request for normal user."
    fi
    if $RUN_UPDATE && ! $DRYRUN; then
	update-grub
    fi
}

function restore_grub {
    cd /etc/default
    if test -f grub.origkp; then
	echo_log "Restore original grub kernel parameter"
	if ! $DRYRUN; then
	    mv grub.origkp grub
	    RUN_UPDATE=true
	fi
    fi
    if test -f grub.origmrk; then
	echo_log "Restore original grub boot menu"
	if ! $DRYRUN; then
	    mv grub.origmrk grub
	    RUN_UPDATE=true
	fi
    fi
    cd /etc/grub.d
    if test -f 10_linux.origmrk; then
	echo_log "Restore original grub linux script"
	if ! $DRYRUN; then
	    mv 10_linux.origmrk 10_linux
	    chmod a+x 10_linux
	    RUN_UPDATE=true
	fi
    fi
    if test -f /boot/grub/grubenv; then
	echo_log "Disable reboot requests for normal user."
	if ! $DRYRUN; then
	    chmod go-w /boot/grub/grubenv
	fi
    fi
    if $RUN_UPDATE && ! $DRYRUN; then
	update-grub
    fi
}


###########################################################################
# udev permissions for comedi:

function setup_comedi {
    if getent group iocard > /dev/null; then
	echo_log "Group \"iocard\" already exist."
    else
	echo_log "Add group \"iocard\"."
	if ! $DRYRUN; then
	    addgroup --system iocard
	fi
    fi
    if test -d /etc/udev/rules.d; then
	if test -f /etc/udev/rules.d/95-comedi.rules; then
	    echo_log "File /etc/udev/rules.d/95-comedi.rules already exist."
	else
	    echo_log "Assign comedi modules to \"iocard\" group via udev rule in \"/etc/udev/rules.d\"."
	    if ! $DRYRUN; then
		{
		    echo "# Add comedi DAQ boards to iocard group."
		    echo "# This file has been created by ${MAKE_RTAI_KERNEL}."
		    echo
		    echo 'KERNEL=="comedi*", MODE="0660", GROUP="iocard"'
		} > /etc/udev/rules.d/95-comedi.rules
		udevadm trigger
	    fi
	    echo_log ""
	    echo_log "You still need to assign users to the \"iocard\" group! Run"
	    echo_log "$ sudo adduser <username> iocard"
	    echo_log "for each user <username> that needs access to the data acquisition boards."
	fi
    else
	echo_log "Directory \"/etc/udev/rules.d\" does not exist - cannot assign comedi modules to iocard group."
    fi
}

function restore_comedi {
    if test -f /etc/udev/rules.d/95-comedi.rules && grep -q "${MAKE_RTAI_KERNEL}" /etc/udev/rules.d/95-comedi.rules; then
	echo_log "Remove comedi device drivers from \"iocard\" group."
	if ! $DRYRUN; then
	    rm /etc/udev/rules.d/95-comedi.rules
	    udevadm trigger
	fi
    fi
}


###########################################################################
# actions:

function setup_features {
    check_root
    if test -z $1; then
	setup_messages
	setup_grub
	setup_comedi
	setup_kernel_param $KERNEL_PARAM
    else
	for TARGET; do
	    case $TARGET in
		messages ) setup_messages ;;
		grub ) setup_grub ;;
		comedi ) setup_comedi ;;
		kernel ) setup_kernel_param $KERNEL_PARAM ;;
		* ) echo "unknown target $TARGET" ;;
	    esac
	done
    fi
}

function restore_features {
    check_root
    if test -z $1; then
	restore_messages
	restore_grub
	restore_comedi
	restore_kernel_param
	restore_test_batch
    else
	for TARGET; do
	    case $TARGET in
		messages ) restore_messages ;;
		grub ) restore_grub ;;
		comedi ) restore_comedi ;;
		kernel ) restore_kernel_param ;;
		testbatch ) restore_test_batch ;;
		* ) echo "unknown target $TARGET" ;;
	    esac
	done
    fi
}

function init_installation {
    check_root
    setup_messages
    setup_grub
    setup_comedi
    download_rtai
    print_full_info rtai
}

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
    echo_log "Please reboot into the ${KERNEL_NAME} kernel by executing"
    echo_log "$ ./${MAKE_RTAI_KERNEL} reboot"
    echo_log
}

function reconfigure {
    RECONFIGURE_KERNEL=true
    check_root

    uninstall_kernel
    ${MAKE_RTAI} && uninstall_rtai

    if ! ( unpack_kernel && patch_kernel && build_kernel && ( ${MAKE_NEWLIB} && build_newlib || MAKE_NEWLIB=false ) && ( ${MAKE_RTAI} && build_rtai || true ) ); then
	echo_log "Failed to reconfigure and build kernel or RTAI"
	return 1
    fi

    ${MAKE_COMEDI} && uninstall_comedi

    if ! ( ${MAKE_COMEDI} && build_comedi || true ); then
	echo_log "Failed to build comedi"
	return 1
    fi

    echo_log
    echo_log "Done!"
    echo_log "Please reboot into the ${KERNEL_NAME} kernel by executing"
    echo_log "$ ./${MAKE_RTAI_KERNEL} reboot"
    echo_log
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
		* ) echo "unknown target $TARGET" ;;
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
		* ) echo "unknown target $TARGET" ;;
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
		* ) echo "unknown target $TARGET" ;;
	    esac
	done
    fi
}

function buildplain_kernel {
    check_root
    unpack_kernel && build_kernel
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
		* ) echo "unknown target $TARGET" ;;
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
		* ) echo "unknown target $TARGET" ;;
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
		* ) echo "unknown target $TARGET" ;;
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
		* ) echo "unknown target $TARGET" ;;
	    esac
	done
    fi
}

function clean_unpack_patch_kernel {
   check_root && clean_kernel && unpack_kernel && patch_kernel
 }

function print_help {
    if test -z "$1"; then
	help_usage
    else
	for TARGET; do
	    case $TARGET in
		info) help_info ;;
		setup) help_setup ;;
		restore) help_setup ;;
		reboot) help_reboot ;;
		test) help_test ;;
		report) help_report ;;
		* ) echo "sorry, no help available for $TARGET" ;;
	    esac
	done
    fi
}

###########################################################################
###########################################################################
# main script:

rm -f "$LOG_FILE"

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

	-d )
	    shift
	    DRYRUN=true
	    ;;
	-s )
	    shift
	    if test -n "$1" && test "x$1" != "xreconfigure"; then
		KERNEL_PATH=$1
		shift
	    else
		echo "you need to specify a path for the kernel sources after the -s option"
		exit 1
	    fi
	    ;;
	-n )
	    shift
	    if test -n "$1" && test "x$1" != "xreconfigure"; then
		KERNEL_NUM=$1
		shift
	    else
		echo "you need to specify a name for the kernel after the -n option"
		exit 1
	    fi
	    ;;
	-r )
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
	    ;;
	-p )
	    shift
	    if test -n "$1" && test "x$1" != "xreconfigure"; then
		RTAI_PATCH=$1
		shift
		RTAI_PATCH_CHANGED=true
	    else
		echo "you need to specify an rtai patch file after the -p option"
		exit 1
	    fi
	    ;;
	-k )
	    shift
	    if test -n "$1" && test "x$1" != "xreconfigure"; then
		LINUX_KERNEL=$1
		shift
		LINUX_KERNEL_CHANGED=true
	    else
		echo "you need to specify a linux kernel version after the -k option"
		exit 1
	    fi
	    ;;
	-c )
	    shift
	    if test -n "$1" && test "x$1" != "xreconfigure"; then
		KERNEL_CONFIG="$1"
		if test "x$KERNEL_CONFIG" != "xdef" && test "x$KERNEL_CONFIG" != "xold" && test "x$KERNEL_CONFIG" != "xmod" && test "x${KERNEL_CONFIG:0:1}" != "x/"; then
		    KERNEL_CONFIG="$PWD/$KERNEL_CONFIG"
		fi
		NEW_KERNEL_CONFIG=true
		shift
	    else
		echo "you need to specify a kernel configuration after the -c option"
		exit 1
	    fi
	    ;;
	-l )
	    shift
	    RUN_LOCALMOD=false
	    ;;
	-D )
	    shift
	    KERNEL_DEBUG=true
	    ;;
	-m )
	    shift
	    RTAI_MENU=true
	    ;;
	-* )
	    echo "unknown options $1"
	    exit 1
    esac
done

if $RTAI_DIR_CHANGED && ! $RTAI_PATCH_CHANGED && ! $LINUX_KERNEL_CHANGED; then
    echo_log "Warning: you changed rtai sources and you might need to adapt the linux kernel version and rtai patch file to it."
    sleep 2
fi

test -n "$KERNEL_NUM" && test "x${KERNEL_NUM:0:1}" != "x-" && KERNEL_NUM="-$KERNEL_NUM"
KERNEL_NAME=${LINUX_KERNEL}-${RTAI_DIR}${KERNEL_NUM}
KERNEL_ALT_NAME=${LINUX_KERNEL}.0-${RTAI_DIR}${KERNEL_NUM}
REALTIME_DIR="/usr/realtime/${KERNEL_NAME}"

ACTION=$1
shift
case $ACTION in

    help ) 
	print_help $@ 
	exit 0
	;;

    version ) 
	print_version
	exit 0
	;;

    info ) if test "x$1" = "xgrub"; then
	    print_grub
	elif test "x$1" = "xsettings"; then
	    print_settings
	    echo
	    echo "You may modify the settings by the respective options, or"
	    echo "by editing the variables directly in the ${MAKE_RTAI_KERNEL} script."
	elif test "x$1" = "xsetup"; then
	    print_setup
	else
	    print_full_info $@ 
	    test "x$1" = "xrtai" && rm -f "$LOG_FILE"
	fi ;;

    reconfigure ) reconfigure ;;

    test ) 
	test_kernel $@
	exit 0 ;;

    report ) 
	test_report $@
	exit 0 ;;

    init) init_installation ;;
    setup ) setup_features $@ ;;
    restore ) restore_features $@ ;;
    download ) download_all $@ ;;
    update ) update_all $@ ;;
    patch ) clean_unpack_patch_kernel ;;
    prepare ) prepare_kernel_configs ;;
    build ) build_all $@ ;;
    buildplain ) buildplain_kernel $@ ;;
    install ) install_all $@ ;;
    clean ) clean_all $@ ;;
    uninstall ) uninstall_all $@ ;;
    remove ) remove_all $@ ;;
    reboot ) reboot_kernel $@ ;;

    * ) if test -n "$1"; then
	    echo "unknown action \"$1\""
	    echo
	    help_usage
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
    rm "$LOG_FILE"
fi

exit $STATUS
