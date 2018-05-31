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
                           # - "old" for oldconfig from the running kernel,
                           # - "def" for the defconfig target,
                           # - "mod" for the localmodconfig target, (even if kernel do not match)
                           # - "backup" for the backed up kernel configuration from the last test batch.
                           # - a kernel config file.
                           # afterwards, the localmodconfig target is executed, 
                           # if the running kernel matches LINUX_KERNEL.
: ${RUN_LOCALMOD:=true}    # run make localmodconf after selecting a kernel configuration (disable with -l)
: ${KERNEL_MENU:="menuconfig"} # the menu for editing the kernel configuration
                               # (menuconfig, gconfig, xconfig)
: ${KERNEL_DEBUG:=false}   # generate debugable kernel (see man crash), set with -D

: ${KERNEL_PARAM:="idle=poll"}      # kernel parameter to be passed to grub
: ${KERNEL_PARAM_DESCR:="idle"}     # one-word description of KERNEL_PARAM 
                                    # used for naming test resutls
: ${BATCH_KERNEL_PARAM:="oops=panic nmi_watchdog=panic panic_on_warn softlockup_panic=1 unknown_nmi_panic panic=-1"} # additional kernel parameter passed to grub for test batch - we want to reboot in any case!
: ${KERNEL_CONFIG_BACKUP:="config-backup"}     # stores initial kernel configuration for test batches

: ${NEWLIB_TAR:=newlib-3.0.0.20180226.tar.gz}  # tar file of current newlib version 
                                               # at ftp://sourceware.org/pub/newlib/index.html
                                               # in case git does not work

: ${MAKE_NEWLIB:=true}        # for automatic targets make newlib library
: ${MAKE_RTAI:=true}          # for automatic targets make rtai library
: ${MAKE_COMEDI:=true}        # for automatic targets make comedi library

: ${RTAI_HAL_PARAM:=""}       # parameter for the rtai_hal module used for testing
: ${RTAI_SCHED_PARAM:=""}     # parameter for the rtai_sched module used for testing
: ${TEST_TIME:=""}            # time in seconds used for latency test
: ${TEST_TIME_DEFAULT:="600"} # default time in seconds used for latency test
: ${STARTUP_TIME:=300}        # time to wait after boot to run a batch test in seconds
: ${COMPILE_TIME:=800}        # time needed for building a kernel with reconfigure
                              # (this is only used for estimating the duration of a test batch)

: ${SHOWROOM_DIR:=showroom}   # target directory for rtai-showrom in ${LOCAL_SRC_PATH}

###########################################################################
# some global variables:

FULL_COMMAND_LINE="$@"

MAKE_RTAI_KERNEL="${0##*/}"
MAKE_RTAI_CONFIG="${MAKE_RTAI_KERNEL%.*}.cfg"
LOG_FILE="${PWD}/${MAKE_RTAI_KERNEL%.*}.log"

VERSION_STRING="${MAKE_RTAI_KERNEL} version 4.0 by Jan Benda, April 2018"
DRYRUN=false                 # only show what is being done (set with -d)
RECONFIGURE_KERNEL=false
NEW_KERNEL_CONFIG=false
DEFAULT_RTAI_DIR="$RTAI_DIR"
RTAI_DIR_CHANGED=false
RTAI_PATCH_CHANGED=false
LINUX_KERNEL_CHANGED=false
RTAI_MENU=false              # enter RTAI configuration menu (set with -m)

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

function indent {
    awk '{print "  " $0}'
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

sudo ${MAKE_RTAI_KERNEL} info [rtai|kernel|cpu|grub|settings|setup|log|configs [<FILE>]]
sudo ${MAKE_RTAI_KERNEL} [-c xxx] info menu

info                 : display properties of rtai patches, loaded kernel modules, kernel, machine,
                       and grub menu (configs and menu targets are excluded)
info rtai            : list all available patches and suggest the one fitting to the kernel
info kernel          : show name and kernel parameter of the currently running kernel
info cpu             : show properties of your CPUs. 
                       Information about c-states is not always available - better check the i7z programm.
info menu            : show kernel configuration menu of the specified (-c) kernel configuration
info grub            : show grub boot menu entries
info settings        : show the values of all configuration variables
info setup           : show modifikations of your system made by ${MAKE_RTAI_KERNEL} (run as root)
info log             : show the content of the log file if available - useful after test batch
info configs         : show available kernel configurations in all files config-*
info configs <FILES> : show kernel configurations contained in <FILES>
info configs > <FILE>: save kernel configurations contained in <FILES>
                       to file <FILE> usable as a test batch file.

-c xxx: specify a kernel configuration xxx:
        - old: use the kernel configuration of the currently running kernel
        - def: generate a kernel configuration using make defconfig
        - mod: simplify existing kernel configuration using make localmodconfig
               even if kernel do not match
        - backup: use the backed up kernel configuration from the last test batch.
        - <config-file>: provide a particular configuration file.
        After setting the configuration (except for mod), make localmodconfig
        is executed to deselect compilation of unused modules, but only if the
        runnig kernel matches the selected kernel version (major.minor only).

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
                   extra RTAI kernel parameter, user can reboot and set rtai kernel parameter)
setup comedi     : create "iocard" group and assign comedi devices to this group
setup kernel     : set kernel parameter for the grub boot menu to "$KERNEL_PARAM"

restore          : restore the original system settings (messages, grub, comedi, kernel, and testbatch)
restore messages : restore the original rsyslog settings
restore grub     : restore the original grub boot menu settings and user access
restore comedi   : do not assign comedi devices to the iocard group
restore kernel   : restore default kernel parameter for the grub boot menu
restore testbatch: uninstall the automatic test script from crontab and
                   remove variables from the grub environment (see help test)

EOF
}

function help_reboot {
    cat <<EOF
$VERSION_STRING

Reboot and set kernel parameter.

Usage:

${MAKE_RTAI_KERNEL} [-d] [-n xxx] [-r xxx] [-k xxx] reboot [keep|none|<XXX>|<FILE>|<N>|default]

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
reboot none   : reboot into rtai kernel ${MAKE_RTAI_KERNEL} is configured for
                without any additional kernel parameter
reboot N      : reboot into a kernel as specified by grub menu entry index N
                without additional kernel parameter,
                see "${MAKE_RTAI_KERNEL} info grub" for the grub menu.
reboot default: reboot into the default kernel of the grub menu.

Rebooting as a regular user (without sudo) has the advantage to store
the session of your window manager. With the grub environment available
(/boot/grub/grubenv) this should be possible (after an "setup grub").

EOF
}

function help_test {
    cat <<EOF
$VERSION_STRING

Test the performance of the rtai-patched linux kernel.

Usage:

sudo ${MAKE_RTAI_KERNEL} [-d] [-n xxx] [-r xxx] [-k xxx] test [[hal|sched|math|comedi] [calib]
     [kern|kthreads|user|all|none] [<NNN>] [auto <XXX> | batch clocks|cstates|acpi|isolcpus|<FILE>]]

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

For a completely automized series of tests of various kernel parameters and kernel configurations
under different loads you may add as the last arguments:
  batch FILE     : automatically run tests with various kernel parameter and configurations as specified in FILE
  batch clocks    : write a default batch file with clock and timer related kernel parameters to be tested
  batch cstates  : write a default batch file with c-states related kernel parameters to be tested
  batch acpi     : write a default batch file with acpi related kernel parameters to be tested
  batch apic     : write a default batch file with apic related kernel parameters to be tested
  batch isolcpus : write a default batch file with load settings and isolcpus kernel parameters to be tested
  batch dma      : write a default batch file with io load, dma, and isolcpus kernel parameters to be tested
This successively reboots into the RTAI kernel with the kernel parameter 
set to the ones specified by the KERNEL_PARAM variable and as specified in FILE,
and runs the tests as specified by the previous commands (without the "auto" command).
Special lines in FILE cause reboots into the default kernel and building an RTAI-patched kernel
with a new configuration.

In a batch FILE
- everything behind a hash ('#') is a comment that is completely ignored
- empty lines are ignored
- a line of the format
  <descr> : <load> : <params>
  describes a configuration to be tested:
  <descr> is a one-word string describing the kernel parameter 
      (a description of the load settings is added automatically to the description)
  <load> defines the load processes to be started before testing (cpu io mem net full, see above)
  <param> is a list of kernel parameter to be used.
- a line of the format
  <descr> : CONFIG : <file>
  specifies a new kernel configuration stored in <file>,
  that is compiled after booting into the default kernel.
  <descr> describes the kernel configuration; it is used for naming successive tests.
  Actually, <file> can be everything the -c otion is accepting, in particular
  <descr> : CONFIG : backup
  compiles a kernel with the configuration of the kernel at the beginning of the tests.
  This is particularly usefull as the last line of a batch file.
- the first line  of the batch file can be just
  <descr> : CONFIG :
  this sets <descr> as the description of the already existing RTAI kernel for the following tests.

Use 
${MAKE_RTAI_KERNEL} prepare
to generate a file with various kernel configurations.

Example lines:
  lowlatency : CONFIG :
  idlenohz : : idle=poll nohz=off
  nodyntics : CONFIG : config-nodyntics
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
        - old: use the kernel configuration of the currently running kernel
        - def: generate a kernel configuration using make defconfig
        - mod: simplify existing kernel configuration using make localmodconfig
               even if kernel do not match
        - backup: use the backed up kernel configuration from the last test batch.
        - <config-file>: provide a particular configuration file
        After setting the configuration (except for mod), make localmodconfig
        is executed to deselect compilation of unused modules, but only if the
        runnig kernel matches the selected kernel version (major.minor only).
-l    : disable call to make localmodconf after a kernel configuration 
        has been selected via the -c switch (RUN_LOCALMOD=${RUN_LOCALMOD})
-D    : generate kernel package with debug symbols in addition (KERNEL_DEBUG=${KERNEL_DEBUG})
-m    : enter the RTAI configuration menu

Note: for running test batches, settings provided via the command line are lost after rebooting.

You can modify the settings by editing "$MAKE_RTAI_KERNEL" directly.
Alternatively, you can provide settings by listing them in a configuration file
in the current working directory called "$MAKE_RTAI_CONFIG". Create a configuration
file by means of the "config" action (see below).

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
  prepare    : prepare kernel configurations for a test batch (no target required)
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
               kernel, cpus, machine, log file, and grub menu
               (run "${MAKE_RTAI_KERNEL} help info" for details)
  config     : write the configuration file \"${MAKE_RTAI_CONFIG}\" 
               that you can edit according to your needs
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

$ sudo ${MAKE_RTAI_KERNEL} test ${TEST_TIME_DEFAULT} batch testclocks.mrk
  automaticlly test all the kernel parameter and kernel configurations specified in the file testclocks.mrk.

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
    echo "System modifications made by ${MAKE_RTAI_KERNEL}:"
    echo
    # messages:
    if test -f /etc/rsyslog.d/50-default.conf.origmrk; then
	echo "messages : /etc/rsyslog.d/50-default.conf is modified"
	echo "           run \"${MAKE_RTAI_KERNEL} restore messages\" to restore"
    elif test -f /etc/rsyslog.d/50-default.conf; then
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
    if test -f /boot/grub/grubenv; then
	if grub-editenv - list | grep -q "rtai_cmdline"; then
	    echo "kernel   : /boot/grub/grubenv is modified ($(grub-editenv - list | grep "rtai_cmdline"))"
	    echo "           run \"${MAKE_RTAI_KERNEL} restore kernel\" to restore"
	else
	    echo "kernel   : /boot/grub/grubenv is not modified"
	    echo "           run \"${MAKE_RTAI_KERNEL} setup kernel\" for setting up RTAI kernel parameter"
	fi
    else
	if test -f /etc/default/grub.origkp; then
	    echo "kernel   : /etc/default/grub is modified"
	    echo "           run \"${MAKE_RTAI_KERNEL} restore kernel\" to restore"
	else
	    echo "kernel   : /etc/default/grub is not modified"
	    echo "           run \"${MAKE_RTAI_KERNEL} setup kernel\" for setting up default RTAI kernel parameter"
	fi
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

function print_log {
    if test -f "${LOG_FILE}"; then
	echo "Content of the log-file \"${LOG_FILE}\":"
	cat "$LOG_FILE"
    else
	echo "Log-file \"${LOG_FILE}\" not available."
    fi
}

function print_kernel_configs {
    KCF=""
    test -z "$1" && KCF="config-*"
    if [ -t 1 ]; then
	echo "Available kernel configurations - add them to a test batch file."
	echo "The asterisk marks configurations that are identical with the current one."
	echo
	for FILE in "$@" $KCF; do
	    SAME="  "
	    diff -q "$FILE" $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/.config> /dev/null && SAME="* "
	    echo "${SAME}${FILE##*config-} : CONFIG : $FILE"
	done
    else
	for FILE in "$@" $KCF; do
	    echo "${FILE##*config-} : CONFIG : $FILE"
	done
    fi
}

function print_grub {
    if test "x$1" = "xenv" && test -r /boot/grub/grubenv; then
	echo "Grub environment:"
	grub-editenv - list | indent
	echo
    fi
    echo "Grub menu entries:"
    IFSORG="$IFS"
    IFS=$'\n'
    N=0
    for gm in $(grep '^\s*menuentry ' /boot/grub/grub.cfg | cut -d "'" -f 2); do
	echo "  $N) $gm"
	let N+=1
    done
    IFS="$IFSORG"
    echo
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
    echo
}

function print_kernel {
    echo "Hostname: $(hostname)"
    echo
    echo "Running kernel (uname -r): $(uname -r)"
    echo
    echo "Kernel parameter (/proc/cmdline):"
    for param in $(cat /proc/cmdline); do
	echo "  $param"
    done
    echo
}

function print_cpus {
    echo "CPU topology, frequencies, and idle states (/sys/devices/system/cpu/*):"
    printf "CPU topology                   CPU frequency scaling              "
    test -r /sys/devices/system/cpu/cpu0/cpuidle/state0/name && printf "  CPU idle states (enabled fraction%%)"
    printf "\n"
    printf "logical  socket  core  online  freq/MHz      governor  transitions"
    if test -f /sys/devices/system/cpu/cpu0/cpuidle/state0/name; then
	for CSTATE in /sys/devices/system/cpu/cpu0/cpuidle/state*/name; do
	    printf "  %-7s" $(cat $CSTATE)
	done
    fi
    printf "\n"
    CSTATEUSAGE="usage" # "usage" or "time"
    CPU_NUM=0
    for CPU in /sys/devices/system/cpu/cpu[0-9]*; do
	let CPU_NUM+=1
	CPUT="$CPU/topology"
	LC_NUMERIC="en_US.UTF-8"
	ONLINE=1
	test -r $CPU/online && ONLINE=$(cat $CPU/online)
	CPUFREQ=$(grep 'cpu MHz' /proc/cpuinfo | awk -F ': ' "NR==$CPU_NUM {print \$2}")
	test -r $CPU/cpufreq/scaling_cur_freq && CPUFREQ=$(echo "scale=3;" $(cat $CPU/cpufreq/scaling_cur_freq)/1000000.0 | bc)
	CPUFREQGOVERNOR="-"
	test -f $CPU/cpufreq/scaling_governor && CPUFREQGOVERNOR=$(cat $CPU/cpufreq/scaling_governor)
	CPUFREQTRANS="n.a."
	test -r $CPU/cpufreq/stats/total_trans && CPUFREQTRANS=$(cat $CPU/cpufreq/stats/total_trans)
	printf "  cpu%-2d  %6d  %4d  %6d  %8.3f  %12s  %11s" ${CPU#/sys/devices/system/cpu/cpu} $(cat $CPUT/physical_package_id) $(cat $CPUT/core_id) $ONLINE $CPUFREQ $CPUFREQGOVERNOR $CPUFREQTRANS
	if test -f $CPU/cpuidle/state0/$CSTATEUSAGE; then
	    SUM=$(cat $CPU/cpuidle/state?/$CSTATEUSAGE | awk '{N+=$1} END {print N}')
	    for CSTATE in $CPU/cpuidle/state*; do
		printf "  %1s %4.1f%%" $(cat $CSTATE/disable) $(echo "scale=1; 100.0*$(cat $CSTATE/$CSTATEUSAGE)/$SUM" | bc)
	    done
	fi
    printf "\n"
    done
    echo

    echo "CPU idle and boost (/sys/devices/system/cpu/{cpuidle,cpufreq}):"
    CPU_IDLE="no"
    if test -r /sys/devices/system/cpu/cpuidle/current_driver; then
	CPU_IDLE="$(cat /sys/devices/system/cpu/cpuidle/current_driver)"
    fi
    CPU_BOOST="no"
    if test -f /sys/devices/system/cpu/cpufreq/boost; then
	CPU_BOOST="$(cat /sys/devices/system/cpu/cpufreq/boost)"
    fi
    echo "  cpuidle driver : $CPU_IDLE"
    echo "  boost          : $CPU_BOOST"
    echo

    if sensors --version &> /dev/null; then
	echo "CPU core temperatures (sensors):"
	sensors | grep Core | indent
	echo
    fi

    MAXCPUFREQ="n.a."
    test -r $CPU/cpufreq/cpuinfo_max_freq && MAXCPUFREQ=$(echo "scale=3;" $(cat $CPU/cpufreq/cpuinfo_max_freq)/1000000.0 | bc)
    test -r $CPU/cpufreq/scaling_max_freq && MAXCPUFREQ=$(echo "scale=3;" $(cat $CPU/cpufreq/scaling_max_freq)/1000000.0 | bc)

    echo "CPU (/proc/cpuinfo):"
    echo "  $(grep "model name" /proc/cpuinfo | awk -F '\t:' 'NR==1 { printf "%-17s : %s", $1, $2}')"
    echo "  number of CPUs    : $CPU_NUM"
    test "MAXCPUFREQ" != "n.a." && echo "  max CPU frequency : $MAXCPUFREQ MHz"
    echo "  CPU family        : $(grep "cpu family" /proc/cpuinfo | awk 'NR==1 { print $4}')"
    echo "  machine (uname -m): $MACHINE"
    echo "  memory (free -h)  : $(free -h | grep Mem | awk '{print $2}') RAM"
    echo
}

function print_distribution {
    if lsb_release &> /dev/null; then
	echo "Distribution (lsb_release -a):"
	lsb_release -a 2> /dev/null | indent
    else
	echo "Distribution: unknown"
    fi
    echo
}

function print_settings {
    echo "Settings of ${VERSION_STRING}:"
    echo "  KERNEL_PATH     (-s) = $KERNEL_PATH"
    echo "  LINUX_KERNEL    (-k) = $LINUX_KERNEL"
    echo "  KERNEL_SOURCE_NAME   = $KERNEL_SOURCE_NAME"
    echo "  KERNEL_NUM      (-n) = $KERNEL_NUM"
    echo "  KERNEL_CONFIG   (-c) = $KERNEL_CONFIG"
    echo "  KERNEL_MENU          = $KERNEL_MENU"
    echo "  RUN_LOCALMOD    (-l) = $RUN_LOCALMOD"
    echo "  KERNEL_DEBUG    (-D) = $KERNEL_DEBUG"
    echo "  KERNEL_PARAM         = $KERNEL_PARAM"
    echo "  KERNEL_PARAM_DESCR   = $KERNEL_PARAM_DESCR"
    echo "  BATCH_KERNEL_PARAM   = $BATCH_KERNEL_PARAM"
    echo "  KERNEL_CONFIG_BACKUP = $KERNEL_CONFIG_BACKUP"
    echo "  TEST_TIME_DEFAULT    = $TEST_TIME_DEFAULT"
    echo "  STARTUP_TIME         = $STARTUP_TIME"
    echo "  COMPILE_TIME         = $COMPILE_TIME"
    echo "  LOCAL_SRC_PATH       = $LOCAL_SRC_PATH"
    echo "  RTAI_DIR        (-r) = $RTAI_DIR"
    echo "  RTAI_PATCH      (-p) = $RTAI_PATCH"
    echo "  RTAI_MENU       (-m) = $RTAI_MENU"
    echo "  RTAI_HAL_PARAM       = $RTAI_HAL_PARAM"
    echo "  RTAI_SCHED_PARAM     = $RTAI_SCHED_PARAM"
    echo "  SHOWROOM_DIR         = $SHOWROOM_DIR"
    echo "  MAKE_NEWLIB          = $MAKE_NEWLIB"
    echo "  MAKE_RTAI            = $MAKE_RTAI"
    echo "  MAKE_COMEDI          = $MAKE_COMEDI"
    echo
}

function print_config {
    echo "# settings for ${MAKE_RTAI_KERNEL}:"
    echo
    echo "# Path for the kernel archive and sources:"
    echo "KERNEL_PATH=\"$KERNEL_PATH\""
    echo
    echo "# Version of linux kernel:"
    echo "LINUX_KERNEL=\"$LINUX_KERNEL\""
    echo
    echo "# Name for kernel source directory to be appended to LINUX_KERNEL:"
    echo "KERNEL_SOURCE_NAME=\"$KERNEL_SOURCE_NAME\""
    echo
    echo "# Name for RTAI patched linux kernel to be appended to kernel and RTAI version:"
    echo "KERNEL_NUM=\"$KERNEL_NUM\""
    echo
    echo "# The kernel configuration to be used:"
    echo "#   \"old\" for oldconfig from the running (or already configured kernel) kernel,"
    echo "#   \"def\" for the defconfig target,"
    echo "#   \"mod\" for the localmodconfig target, (even if kernel versions do not match)"
    echo "#   \"backup\" for the backed up kernel configuration from the last test batch."
    echo "# or a full path to a config file."
    echo "KERNEL_CONFIG=\"$KERNEL_CONFIG\""
    echo
    echo "# Menu for editing the kernel configuration (menuconfig, gconfig, xconfig):"
    echo "KERNEL_MENU=\"$KERNEL_MENU\""
    echo
    echo "# Run localmodconfig on the kernel configuration to deselect"
    echo "# all kernel modules that are not currently used."
    echo "RUN_LOCALMOD=$RUN_LOCALMOD"
    echo
    echo "# Generate debug packe of the kernel (not tested...)?"
    echo "KERNEL_DEBUG=$KERNEL_DEBUG"
    echo
    echo "# Default kernel parameter to be used when booting into the RTAI patched kernel:"
    echo "KERNEL_PARAM=\"$KERNEL_PARAM\""
    echo
    echo "# One-word description of the KERNEL_PARAM that is added to the test description:"
    echo "KERNEL_PARAM_DESCR=\"$KERNEL_PARAM_DESCR\""
    echo
    echo "# Kernel parameter to be added when running test batches:"
    echo "BATCH_KERNEL_PARAM=\"$BATCH_KERNEL_PARAM\""
    echo
    echo "# Name of the file used by test batches for backing up the kernel configuration:"
    echo "KERNEL_CONFIG_BACKUP=\"$KERNEL_CONFIG_BACKUP\""
    echo
    echo "# Default time in seconds to run the RTAI latency tests:"
    echo "TEST_TIME_DEFAULT=$TEST_TIME_DEFAULT"
    echo
    echo "# Time in seconds to wait for starting a test batch after reboot:"
    echo "STARTUP_TIME=$STARTUP_TIME"
    echo
    echo "# Approximate time in seconds needed to compile a linux kernel"
    echo "# (watch output of ${MAKE_RTAI_KERNEL} reconfigure for a hint):"
    echo "COMPILE_TIME=$COMPILE_TIME"
    echo
    echo "# Base path for sources of RTAI, newlib, and comedi:"
    echo "LOCAL_SRC_PATH=\"$LOCAL_SRC_PATH\""
    echo
    echo "# Name of source and folder of RTAI sources in LOCAL_SOURCE_PATH:"
    echo "# official relases for download (www.rtai.org):"
    echo "# - rtai-4.1: rtai release version 4.1"
    echo "# - rtai-5.1: rtai release version 5.1"
    echo "# - rtai-x.x: rtai release version x.x"
    echo "# from cvs (http://cvs.gna.org/cvsweb/?cvsroot=rtai):"
    echo "# - magma: current development version"
    echo "# - vulcano: stable development version"
    echo "RTAI_DIR=\"$RTAI_DIR\""
    echo
    echo "# File name of RTAI patch to be used (check with ${MAKE_RTAI_KERNEL} info rtai):"
    echo "RTAI_PATCH=\"$RTAI_PATCH\""
    echo
    echo "# Bring up menu for configuring RTAI?"
    echo "RTAI_MENU=$RTAI_MENU"
    echo
    echo "# Parameter to be passed on to the rtai_hal kernel module:"
    echo "RTAI_HAL_PARAM=\"$RTAI_HAL_PARAM\""
    echo
    echo "# Parameter to be passed on to the rtai_sched kernel module:"
    echo "RTAI_SCHED_PARAM=\"$RTAI_SCHED_PARAM\""
    echo
    echo "# Name of folder for showroom sources in LOCAL_SOURCE_PATH:"
    echo "SHOWROOM_DIR=\"$SHOWROOM_DIR\""
    echo
    echo "# Build newlib math library?"
    echo "MAKE_NEWLIB=$MAKE_NEWLIB"
    echo
    echo "# Build RTAI?"
    echo "MAKE_RTAI=$MAKE_RTAI"
    echo
    echo "# Build comedi daq-board drivers?"
    echo "MAKE_COMEDI=$MAKE_COMEDI"
}

function print_kernel_info {
    echo
    echo "Loaded modules (lsmod):"
    if test -f lsmod.dat; then
	cat lsmod.dat | indent
	rm -f lsmod.dat
    else
	lsmod | indent
    fi
    echo
    echo "Interrupts (/proc/interrupts):"
	cat /proc/interrupts | indent
    echo
    print_distribution
    print_kernel
    print_cpus
    print_versions
    print_grub
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
    PACKAGES="make gcc libncurses-dev zlib1g-dev g++ bc cvs git autoconf automake libtool"
    if test ${LINUX_KERNEL:0:1} -gt 3; then
	PACKAGES="$PACKAGES libssl-dev libpci-dev libsensors4-dev"
    fi
    if $MAKE_COMEDI; then
	# XXX what if libgsl-dev is installed instead of libgsl0-dev ???
	PACKAGES="$PACKAGES bison flex libgsl0-dev libboost-program-options-dev"
    fi
    OPT_PACKAGES="kernel-package stress lm-sensors"
    if $DRYRUN; then
	echo_log "apt-get -y install $PACKAGES"
	for PKG in $OPT_PACKAGES; do
	    echo_log "apt-get -y install $PKG"
	done
    else
	if ! apt-get -y install $PACKAGES; then
	    FAILEDPKGS=""
	    for PKG in $PACKAGES; do
		if ! apt-get -y install $PKG; then
		    FAILEDPKGS="$FAILEDPKGS $PKG"
		fi
	    done
	    if test -n "$FAILEDPKGS"; then
		echo_log "Failed to install missing packages!"
		echo_log "Maybe package names have changed ..."
		echo_log "We need the following packes, try to install them manually:"
		for PKG in $FAILEDPKGS; do
		    echo_log "  $PKG"
		done
		return 1
	    fi
	fi
	if ! apt-get -y install $OPT_PACKAGES; then
	    FAILEDPKGS=""
	    for PKG in $OPT_PACKAGES; do
		if ! apt-get -y install $PKG; then
		    FAILEDPKGS="$FAILEDPKGS $PKG"
		fi
	    done
	    if test -n "$FAILEDPKGS"; then
		echo_log "Failed to install optional packages!"
		echo_log "Maybe package names have changed ..."
		echo_log "Try to install them manually:"
		for PKG in $FAILEDPKGS; do
		    echo_log "  $PKG"
		done
		return 1
	    fi
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
	RTAI_PATCH_SET=${RTAI_PATCH}
	LINUX_KERNEL_SET=${LINUX_KERNEL}
	cd ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/
	echo_log
	echo_log "Available patches for this machine ($RTAI_MACHINE), most latest last:"
	ls -rt -1 *.patch 2> /dev/null | tee -a "$LOG_FILE" | indent
	echo_log
	LINUX_KERNEL_V=${LINUX_KERNEL%.*}
	echo_log "Available patches for the selected kernel's kernel version ($LINUX_KERNEL_V):"
	ls -rt -1 *-${LINUX_KERNEL_V}*.patch 2> /dev/null | tee -a "$LOG_FILE" | indent
	echo_log
	echo_log "Available patches for the selected kernel ($LINUX_KERNEL):"
	ls -rt -1 *-${LINUX_KERNEL}*.patch 2> /dev/null | tee -a "$LOG_FILE" | indent
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
	echo_log "Set values:"
	echo_log
	echo_log "  RTAI_PATCH=\"${RTAI_PATCH_SET}\""
	echo_log "  LINUX_KERNEL=\"${LINUX_KERNEL_SET}\""
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
	echo_log "Path to kernel sources $KERNEL_PATH does not exist!"
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
	    if ! wget https://www.kernel.org/pub/linux/kernel/v${LINUX_KERNEL:0:1}.x/linux-$LINUX_KERNEL.tar.xz; then
		echo_log "Failed to download linux kernel \"https://www.kernel.org/pub/linux/kernel/v${LINUX_KERNEL:0:1}.x/linux-$LINUX_KERNEL.tar.xz\"!"
		return 1
	    fi
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
	    cd - &> /dev/null
	    return 1
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
    cd - &> /dev/null

    # standard softlink to kernel:
    if ! $DRYRUN; then
	cd /usr/src
	ln -sfn $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} linux
	cd - &> /dev/null
    fi
}

function patch_kernel {
    cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
    if $NEW_KERNEL; then
	if ! check_kernel_patch; then
	    cd - &> /dev/null
	    return 1
	fi
	echo_log "apply rtai patch $RTAI_PATCH to kernel sources"
	if ! $DRYRUN; then
	    if ! patch -p1 < ${LOCAL_SRC_PATH}/${RTAI_DIR}/base/arch/$RTAI_MACHINE/patches/$RTAI_PATCH; then
		echo_log "Failed to patch the linux kernel \"$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}\"!"
		cd - &> /dev/null
		return 1
	    fi
	fi
    fi
    cd - &> /dev/null
}

function prepare_kernel_configs {
    check_root
    if ! test -d $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo "Linux kernel path \"$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}\" does not exist."
	echo "Please specify an existing directory with a linux kernel source."
	exit 1
    fi

    echo "Prepare kernel configurations to be tested in a test batch."
    echo
    STEP=0

    let STEP+=1
    echo "Step $STEP: backup the original kernel configuration."
    if ! $DRYRUN; then
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	cp .config .config.origmrk
	cd - > /dev/null
    fi
    echo

    let STEP+=1
    echo "Step $STEP: decide on a configuration mode."
    echo "        - Incremental configurations go on with the changed configurations."
    echo "        - Absolute configurations always start put from the backed up original kernel configuration."
    read -p "        Incremental or Absolute configurations, or Cancel? (i/A/c) " MODE
    ABSOLUTE=false
    case $MODE in
	i|I ) ABSOLUTE=false ;;
	a|A ) ABSOLUTE=true ;;
	'' ) ABSOLUTE=true ;;
	* ) echo_log ""
	    echo_log "Aborted"
	    if ! $DRYRUN; then
		cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
		rm .config.origmrk
		cd - > /dev/null
	    fi
	    exit 0
	    ;;
    esac
    echo

    if $NEW_KERNEL_CONFIG; then
	let STEP+=1
	echo "Step $STEP: set initial kernel configuration from command line."
	echo
	WORKING_DIR="$PWD"
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	config_kernel "$WORKING_DIR"
	cd - > /dev/null
	echo
    fi

    let STEP+=1
    echo "Step $STEP: modify and store the kernel configurations."
    CONFIG_FILES=()
    if ! $DRYRUN; then
	while true; do
	    cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    $ABSOLUTE && cp .config.origmrk .config
	    make $KERNEL_MENU
	    DESCRIPTION=""
	    echo
	    read -p "  Short description of the kernel configuration (empty: finish) " DESCRIPTION
	    echo
	    if test -z "$DESCRIPTION"; then
		break
	    fi
	    make olddefconfig
	    CONFIG_FILES+=( "config-${DESCRIPTION}" )
	    cd - > /dev/null
	    cp $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/.config "config-${DESCRIPTION}"
	    echo "  Saved kernel configuration \"$DESCRIPTION\" to file \"config-${DESCRIPTION}\"."
	done
	cd - > /dev/null
    fi
    echo

    # clean up:
    let STEP+=1
    echo "Step $STEP: restore the original kernel configuration."
    if ! $DRYRUN; then
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	cp .config.origmrk .config
	cd - > /dev/null
    fi
    echo

    let STEP+=1
    if test ${#CONFIG_FILES[@]} -gt 0; then
	echo "Step $STEP: saved ${#CONFIG_FILES[@]} kernel configuration(s)."
	echo
	let STEP+=1
	echo "Step $STEP: go on and use the kernel configurations"
	echo "        by adding the following lines to a test batch file:"
	echo
	for FILE in "${CONFIG_FILES[@]}"; do
	    echo "  ${FILE##*config-} : CONFIG : $FILE"
	done
	echo
	echo "You may pipe these lines directly into a <FILE> that you then can use for a test batch:"
	echo "\$ ${MAKE_RTAI_KERNEL} info configs > <FILE>"
	echo
    else
	echo "Step $STEP: did not save any kernel configurations."
	echo
    fi
    exit 0
}

function config_kernel {
    WORKING_DIR="$1"
    if $NEW_KERNEL_CONFIG; then
	# kernel configuration:
	if test "x$KERNEL_CONFIG" = "xdef"; then
	    echo_log "Use default configuration of kernel (defconfig)."
	    if ! $DRYRUN; then
		make defconfig
	    fi
	elif test "x$KERNEL_CONFIG" = "xold"; then
	    CF="/boot/config-${CURRENT_KERNEL}"
	    test -f "$CF" || CF="/lib/modules/$(uname -r)/build/.config"
	    echo_log "Use configuration from running kernel ($CF) and run olddefconfig."
	    if ! $DRYRUN; then
		cp $CF .config
		make olddefconfig
	    fi
	elif test "x$KERNEL_CONFIG" = "xmod"; then
	    echo_log "Run make localmodconfig."
	    if test ${CURRENT_KERNEL:0:${#LINUX_KERNEL}} != $LINUX_KERNEL ; then
		echo_log "Warning: kernel versions do not match (selected kernel is $LINUX_KERNEL, running kernel is $CURRENT_KERNEL)!"
		echo_log "Run make localmodconfig anyways"
	    fi
	    if ! $DRYRUN; then
		make localmodconfig
	    fi
	    RUN_LOCALMOD=false
	else
	    KCF=""
	    BKP=""
	    if test "x$KERNEL_CONFIG" = "xbackup"; then
		KCF="$KERNEL_CONFIG_BACKUP"
		BKP="backup-"
	    else
		KCF="$KERNEL_CONFIG"
	    fi
	    if test -f "$WORKING_DIR/$KCF"; then
		echo_log "Use ${BKP}configuration from \"$KCF\" and run olddefconfig."
		if ! $DRYRUN; then
		    cp "$WORKING_DIR/$KCF" .config
		    make olddefconfig
		fi
	    else
		echo_log "Unknown kernel configuration file \"$WORKING_DIR/$KCF\"."
		return 1
	    fi
	fi

	if $RUN_LOCALMOD; then
	    if test ${CURRENT_KERNEL%.*} = ${LINUX_KERNEL%.*} ; then
		echo_log "Run make localmodconfig"
		if ! $DRYRUN; then
		    yes "" | make localmodconfig
		fi
	    else
		echo_log "Cannot run make localmodconfig, because kernel version does not match (running kernel: ${CURRENT_KERNEL}, selected kernel: ${LINUX_KERNEL})"
	    fi
	fi
    else
	echo_log "Keep already existing .config file for linux-${KERNEL_NAME}."
    fi
}

function menu_kernel {
    check_root
    if test -d "$KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"; then
	WORKING_DIR="$PWD"
	cd $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}

	echo_log "Backup kernel configuration."
	$DRYRUN || cp .config .config.origmrk

	KF=$KERNEL_CONFIG
	$NEW_KERNEL_CONFIG || KF=".config"
	echo_log "Show kernel configuration menu for configuration \"$KF\"."
	config_kernel "$WORKING_DIR"
	if ! $DRYRUN; then
	    make $KERNEL_MENU
	    mv .config.origmrk .config
	fi
	echo_log "Restored original kernel configuration."
    else
	echo_log "Directory $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME} does not exist."
    fi
}

function build_kernel {
    WORKING_DIR="$PWD"
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

	config_kernel "$WORKING_DIR"

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
		    echo "Run make olddefconfig"
		    make olddefconfig
		else
		    echo "Run make $KERNEL_MENU"
		    make $KERNEL_MENU
		fi
		make deb-pkg LOCALVERSION=-${RTAI_DIR}${KERNEL_NUM} KDEB_PKGVERSION=$(make kernelversion)-1
		# [TAR] creates a tar archive of the sources at the root of the kernel source tree
	    fi
 	    if test "x$?" != "x0"; then
		echo_log
		echo_log "Error: failed to build the kernel!"
		echo_log "Scroll up to see why."
		cd "WORKING_DIR"
		return 1
	    fi
	fi

	cd "WORKING_DIR"

	# install:
	install_kernel || return 1
    else
	echo_log "Keep already compiled linux ${KERNEL_NAME} kernel."
	cd "WORKING_DIR"
    fi
}

function clean_kernel {
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}; then
	echo_log "remove kernel sources $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}"
	if ! $DRYRUN; then
	    rm -r linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}
	    rm -f linux
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
	    if ! dpkg -i "$KERNEL_PACKAGE"; then
		echo_log "Failed to install linux kernel from $KERNEL_PACKAGE !"
		cd - &> /dev/null
		return 1
	    fi
	    if $KERNEL_DEBUG; then
		if ! dpkg -i "$KERNEL_DEBUG_PACKAGE"; then
		    echo_log "Failed to install linux kernel from $KERNEL_DEBUG_PACKAGE !"
		    cd - &> /dev/null
		    return 1
		fi
	    fi
	fi
    else
	echo_log "no kernel to install"
	cd - &> /dev/null
	return 1
    fi
    cd - &> /dev/null
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
	if ! apt-get -y remove linux-image-${KERNEL_NAME}; then
	    if ! apt-get -y remove linux-image-${KERNEL_ALT_NAME}; then
		echo_log "Failed to uninstall linux kernel package \"linux-image-${KERNEL_NAME}\"!"
		return 1
	    fi
	fi
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
    if test -f /boot/grub/grubenv; then
	if ! $DRYRUN; then
	    grub-editenv - set rtai_cmdline="$*"
	fi
	if test -n "$*"; then
	    echo_log "Set RTAI kernel parameter to \"$*\"."
	fi
    elif test -f /etc/default/grub; then
	echo_log "/boot/grub/grubenv not found: try /etc/default/grub"
	check_root
	if ! $DRYRUN; then
	    cd /etc/default
	    test -f grub.origkp && mv grub.origkp grub
	    cp grub grub.origkp
	    sed -e '/GRUB_CMDLINE_RTAI/s/=".*"/="'"$*"'"/' grub.origkp > grub
	    update-grub
	fi
	if test -n "$*"; then
	    echo_log "Set RTAI kernel parameter to \"$*\"."
	fi
    else
	echo_log "/boot/grub/grubenv and /etc/default/grub not found: cannot set RTAI kernel parameter"
    fi
}

function restore_kernel_param {
    if test -f /boot/grub/grubenv; then
	echo_log "Remove RTAI kernel parameter from grubenv."
	if ! $DRYRUN; then
	    grub-editenv - unset rtai_cmdline
	fi
    fi
    if test -f /etc/default/grub.origkp; then
	echo_log "Restore original RTAI kernel parameter in /etc/default/grub."
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

function reboot_unset_kernel {
    # make sure to boot into default kernel:
    if test -f /boot/grub/grubenv; then
	if grub-editenv - list | grep -q next_entry; then
	    if ! $DRYRUN; then
		grub-editenv - unset next_entry
		echo_log "unset next_entry from grubenv file"
	    fi
	fi
	echo_log "reboot into default grub menu entry"
    else
	echo_log "failed to reset default boot kernel (/boot/grub/grubenv not available)"
    fi
}

function reboot_cmd {
# reboot the computer

# reboot -f   # cold start
# reboot      # calls shutdown -r
# shutdown brings the system into the reuested runlevel (init 0/6)
# shutdown -r # reboot in a minute   
# shutdown -r now
# shutdown -r +<minutes>

    echo_log "reboot now"
    echo_log "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
    if ! $DRYRUN; then
	if test "x$1" = "xcold"; then
	    echo_kmsg "REBOOT (reboot -f)"
	    reboot -f
	elif test "x$(id -u)" != "x0"; then
	    if qdbus --version &> /dev/null; then
		qdbus org.kde.ksmserver /KSMServer org.kde.KSMServerInterface.logout 0 1 2 && return
	    fi
	    if gnome-session-quit --version &> /dev/null; then
		gnome-session-quit --reboot --force && return
	    fi
	    check_root
	    shutdown -r now
	else
	    echo_kmsg "REBOOT (shutdown -r now)"
	    shutdown -r now
	fi
    fi
}

function reboot_kernel {
# default: boot into default kernel
# N      : boot into Nth kernel
# keep   : boot into rtai kernel and keep previous set kernel parameter 
# none   : boot into rtai kernel without additional kernel parameter
# ""     : boot into rtai kernel with additional kernel parameter as specified by KERNEL_PARAM
# XXX    : boot into rtai kernel with additional kernel parameter XXX
# FILE   : boot into rtai kernel with kernel parameter taken from test results file FILE
    echo_log ""
    case $1 in
	default)
	    reboot_unset_kernel
	    sleep 2
	    reboot_cmd
	    ;;

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

	none)
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
    exit 0
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
	echo "Load: $(cut -d ' ' -f 1-3 /proc/loadavg)"
	if test -f load.dat; then
	    sed -e 's/^[ ]*load[ ]*/  /' load.dat
	fi
	echo
	# original test results:
	for TD in kern kthreads user; do
	    for TN in latency switches preempt; do
		TEST_RESULTS=results-$TD-$TN.dat
		if test -f "$TEST_RESULTS"; then
		    echo "$TD/$TN test:"
		    sed -e '/^\*/d' $TEST_RESULTS
		    echo "----------------------------------------"
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
	echo_log "running $DIR/$TEST test"
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
	    mem) LOADMODE="$LOADMODE mem" ;;
	    net) LOADMODE="$LOADMODE net" ;;
	    full) LOADMODE="cpu io mem net" ;;
	    [0-9]*) TEST_TIME="$((10#$1))" ;;
	    auto) shift; test -n "$1" && { DESCRIPTION="$1"; TESTSPECS="$TESTSPECS $1"; } ;;
	    batch) shift; test_batch "$1" "$TEST_TIME" "$TESTMODE" ${TESTSPECS% batch} ;;
	    batchscript) shift; test_batch_script ;;
	    *) echo_log "test $1 is invalid"
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
	lsmod | grep -q rtai_$MOD && { rmmod rtai_$MOD && echo_log "removed already loaded rtai_$MOD"; }
    done
    echo_log

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
	lsmod | grep -q rtai_hal || { insmod ${REALTIME_DIR}/modules/rtai_hal.ko $RTAI_HAL_PARAM && echo_log "loaded  rtai_hal $RTAI_HAL_PARAM" || RTAIMOD_FAILED=true; }
	$RTAIMOD_FAILED || PROGRESS="${PROGRESS}h"
    fi

    # rtai_sched:
    if test $MAXMODULE -ge 2; then
	TESTED="${TESTED}s"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	echo_kmsg "INSMOD ${REALTIME_DIR}/modules/rtai_sched.ko $RTAI_SCHED_PARAM"
	lsmod | grep -q rtai_sched || { insmod ${REALTIME_DIR}/modules/rtai_sched.ko $RTAI_SCHED_PARAM && echo_log "loaded  rtai_sched $RTAI_SCHED_PARAM" || RTAIMOD_FAILED=true; }
	$RTAIMOD_FAILED || PROGRESS="${PROGRESS}s"
    fi

    # rtai_math:
    if test $MAXMODULE -ge 3; then
	if test -f ${REALTIME_DIR}/modules/rtai_math.ko; then
	    TESTED="${TESTED}m"
	    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	    echo_kmsg "INSMOD ${REALTIME_DIR}/modules/rtai_math.ko"
	    lsmod | grep -q rtai_math || { insmod ${REALTIME_DIR}/modules/rtai_math.ko && echo_log "loaded  rtai_math" && PROGRESS="${PROGRESS}m"; }
	else
	    echo_log "rtai_math is not available"
	fi
    fi
    
    if test $MAXMODULE -ge 4 && $MAKE_COMEDI && ! $RTAIMOD_FAILED; then
	TESTED="${TESTED}c"
	test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
	# loading comedi:
	echo_kmsg "LOAD COMEDI MODULES"
	echo_log "triggering comedi "
	udevadm trigger
	sleep 1
	modprobe kcomedilib && echo_log "loaded  kcomedilib"

	lsmod | grep -q kcomedilib && PROGRESS="${PROGRESS}c"
	
	lsmod > lsmod.dat
	
	echo_kmsg "REMOVE COMEDI MODULES"
	remove_comedi_modules
    fi
    test_save "$NAME" "$REPORT" "$TESTED" "$PROGRESS"
    
    # remove rtai modules:
    if test $MAXMODULE -ge 3; then
	echo_kmsg "RMMOD rtai_math"
	lsmod | grep -q rtai_math && { rmmod rtai_math && echo_log "removed rtai_math"; }
    fi
    if test $MAXMODULE -ge 2; then
	echo_kmsg "RMMOD rtai_sched"
	lsmod | grep -q rtai_sched && { rmmod rtai_sched && echo_log "removed rtai_sched"; }
    fi
    if test $MAXMODULE -ge 1; then
	echo_kmsg "RMMOD rtai_hal"
	lsmod | grep -q rtai_hal && { rmmod rtai_hal && echo_log "removed rtai_hal"; }
    fi

    # loading modules failed:
    if $RTAIMOD_FAILED; then
	echo_log "Failed to load RTAI modules."
	echo_log
	if test -z "$DESCRIPTION"; then
	    read -p 'Save configuration? (y/N): ' SAVE
	    if test "$SAVE" = "y"; then
		echo_log
		echo_log "saved kernel configuration in: config-$REPORT"
		echo_log "saved test results in        : latencies-$REPORT"
	    else
		rm -f config-$REPORT
		rm -f latencies-$REPORT
	    fi
	fi
	return
    fi
    echo_log "successfully loaded and unloaded rtai modules"
    echo_log

    # RTAI tests:
    if test "$TESTMODE" != none; then
	# stress program available?
	STRESS=false
	stress --version &> /dev/null && STRESS=true
	# produce load:
	JOB_NUM=$CPU_NUM
	LOAD_JOBS=$(echo $LOADMODE | wc -w)
	if test $LOAD_JOBS -gt 1; then
	    let JOB_NUM=$CPU_NUM/$LOAD_JOBS
	    test $JOB_NUM -le 1 && JOB_NUM=2
	fi
	LOAD_PIDS=()
	LOAD_FILES=()
	test -n "$LOADMODE" && echo_log "start some jobs to produce load:"
	for LOAD in $LOADMODE; do
	    case $LOAD in
		cpu) if $STRESS; then
	                echo_log "  load cpu: stress -c $JOB_NUM" | tee -a load.dat
			stress -c $JOB_NUM &> /dev/null &
			LOAD_PIDS+=( $! )
                    else
	                echo_log "  load cpu: seq $JOB_NUM | xargs -P0 -n1 md5sum /dev/urandom" | tee -a load.dat
			seq $JOB_NUM | xargs -P0 -n1 md5sum /dev/urandom & 
			LOAD_PIDS+=( $! )
                    fi
		    ;;
		io) if $STRESS; then
	                echo_log "  load io : stress --hdd-bytes 128M -d $JOB_NUM" | tee -a load.dat
			stress --hdd-bytes 128M -d $JOB_NUM &> /dev/null &
			LOAD_PIDS+=( $! )
		    else
		        echo_log "  load io : ls -lR" | tee -a load.dat
			while true; do ls -lR / &> load-lsr; done & 
			LOAD_PIDS+=( $! )
			LOAD_FILES+=( load-lsr )
			echo_log "  load io : find" | tee -a load.dat
			while true; do find / -name '*.so' &> load-find; done & 
			LOAD_PIDS+=( $! )
			LOAD_FILES+=( load-find )
		    fi
		    ;;
		mem) if $STRESS; then
	                echo_log "  load mem: stress -m $JOB_NUM" | tee -a load.dat
			stress -m $JOB_NUM &> /dev/null &
			LOAD_PIDS+=( $! )
		    else
		        echo_log "  load mem: no test available"
		    fi
		    ;;
		net) echo_log "  load net: ping -f localhost" | tee -a load.dat
		    ping -f localhost > /dev/null &
		    LOAD_PIDS+=( $! )
		    ;;
		snd) echo_log "  load snd: not implemented yet!" ;;
	    esac
	done
	test -n "$LOADMODE" && echo_log

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
	echo_log "finished all tests"
	echo_log
    fi

    # clean up load:
    for PID in ${LOAD_PIDS[@]}; do
	kill -KILL $PID $(ps -o pid= --ppid $PID)
    done
    for FILE in ${LOAD_FILES[@]}; do
	rm -f $FILE
    done
    # clean up:
    for MOD in msg mbx sem math sched hal; do
	lsmod | grep -q rtai_$MOD && { rmmod rtai_$MOD && echo_log "removed loaded rtai_$MOD"; }
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
	echo_log
	echo_log "saved kernel configuration in : config-$REPORT"
	echo_log "saved test results in         : latencies-$REPORT"
    else
	echo_log "test results not saved"
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

    # write default batch files:
    if ! test -f "$BATCH_FILE"; then
	DEFAULT_BATCHES="clocks cstates acpi apic isolcpus dma"
	for DEFAULT_BATCH in $DEFAULT_BATCHES; do
	    if test "$BATCH_FILE" = "$DEFAULT_BATCH"; then
		BATCH_FILE=test${DEFAULT_BATCH}.mrk
		if test -f $BATCH_FILE; then
		    echo "File \"$BATCH_FILE\" already exists."
		    echo "Cannot write batch file for ${DEAFULT_BATCH} kernel parameter."
		    exit 1
		fi

		case $DEFAULT_BATCH in
		    clocks) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# Batch file for testing RTAI kernel with various kernel parameter related to clocks and timers.
#
# Each line has the format:
# <description> : <load specification> : <kernel parameter>
# for specifying tests, where <description> is a brief one-word description of the kernel
# parameters that is added to the KERNEL_PARAM_DESCR variable.  
# The <kernel parameter> are added to the ones defined in the KERNEL_PARAM variable.
#
# Alternatively, lines of the following format specify a new kernel to be compiled:
# <description> : CONFIG : <config-file>
# <description> : CONFIG : backup
# where <config-file> is the file with the kernel configuration, 
# "backup" specifies the kernel configuration at the beginning of the tests,
# and <description> describes the kernel configuration for the following tests. 
# A line without a configuration file:
# <description> : CONFIG :
# just gives the current kernel configuration the name <description>.
#
# Edit this file according to your needs.
#
# Then run
#
# $ ./$MAKE_RTAI_KERNEL test math ${DEAFULT_BATCH} batch $BATCH_FILE
#
# for testing all the kernel parameter.
#
# The test results are recorded in the latencies-${LINUX_KERNEL}-${RTAI_DIR}-$(hostname)* files.
#
# Generate and view a summary table of the test results by calling
#
# $ ./$MAKE_RTAI_KERNEL report | less -S

# without additional kernel parameter:
plain1 : :

# clocks and timers:
nohz : : nohz=off
tscreliable : : tsc=reliable
tscnoirqtime : : tsc=noirqtime
highresoff : : highres=off
#nolapictimer : : nolapic_timer  # no good
clocksourcehpet : : clocksource=hpet
clocksourcetsc : : clocksource=tsc
hpetdisable : : hpet=disable
skewtick : : skew_tick=1

# test again to see variability of results:
plain2 : :
EOF
			;;

		    cstates) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# Batch file for testing RTAI kernel with kernel parameter related to processor c-states.

# c-states:
plain : :
idlepoll : : idle=poll
idlehalt : : idle=halt
intelcstate1 : : intel_idle.max_cstate=1
processorcstate1 : : intel_idle.max_cstate=0 processor.max_cstate=1
processorcstate0 : : intel_idle.max_cstate=0 processor.max_cstate=0
EOF
			;;

		    acpi) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# Batch file for testing RTAI kernel with various kernel parameter related to acpi.

# acpi:
plain : :
#acpioff : : acpi=off    # often very effective, but weired system behavior
acpinoirq : : acpi=noirq
pcinoacpi : : pci=noacpi
pcinomsi : : pci=nomsi
nopstate : : intel_pstate=disable
EOF
			;;

		    apic) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# Batch file for testing RTAI kernel with various kernel parameter related to apic.

# apic:
plain : :
noapic : : noapic
nox2apic : : nox2apic
x2apicphys : : x2apic_phys
lapic : : lapic
#nolapic : : nolapic    # we need the lapic timer!
#nolapic_timer : : nolapic_timer    # we need the lapic timer!
lapicnotscdeadl : : lapic=notscdeadline
EOF
			;;

		    isolcpus) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# Batch file for testing RTAI kernel with cpu isolation.
# Replace "=1" by the index of the cpu you want to isolate.

plain : :
plain : full :

# isolcpus:
isolcpus : : isolcpus=1
isolcpus : full : isolcpus=1

# isolcpus + nohz_full
isolcpus-nohz : : isolcpus=1 nohz_full=1
isolcpus-nohz : full : isolcpus=1 nohz_full=1

# isolcpus + nohz_full +  rcu_nocbs
isolcpus-nohz-rcu : : isolcpus=1 nohz_full=1 rcu_nocbs=1
isolcpus-nohz-rcu : full : isolcpus=1 nohz_full=1 rcu_nocbs=1
EOF
			;;

		    dma) cat <<EOF > $BATCH_FILE
# $VERSION_STRING
# batch file for testing RTAI kernel with cpu isolation

plain : io :
dma : io : libata.dma=0

# isolcpus:
isolcpus : io : isolcpus=1
dma-isolcpus : io : libata.dma=0 isolcpus=1

# isolcpus + nohz_full
isolcpus-nohz : io : isolcpus=1 nohz_full=1
dma-isolcpus-nohz : io : libata.dma=0 isolcpus=1 nohz_full=1

# isolcpus + nohz_full +  rcu_nocbs
isolcpus-nohz : io : isolcpus=1 nohz_full=1 rcu_nocbs=1
dma-isolcpus-nohz : io : libata.dma=0 isolcpus=1 nohz_full=1 rcu_nocbs=1
EOF
			;;
		esac

		chown --reference=. $BATCH_FILE
		echo "Wrote default kernel parameter to be tested into file \"$BATCH_FILE\"."
		echo ""
		echo "Call test batch again with something like"
		echo "$ sudo ./${MAKE_RTAI_KERNEL} test ${TEST_TIME_DEFAULT} batch $BATCH_FILE"
		exit 0
	    fi
	done
	echo "File \"$BATCH_FILE\" does not exist!"
	exit 1
    fi

    # run batch file:
    N_TESTS=$(sed -e 's/ *#.*$//' $BATCH_FILE | grep -c ':.*:')
    if test $N_TESTS -eq 0; then
	echo_log "No valid configurations specified in file \"$BATCH_FILE\"!"
	exit 1
    fi
    N_COMPILE=$(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | grep -c CONFIG)

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
	TEST_TIME="${TEST_TIME_DEFAULT}"
	TEST_SPECS="$TEST_SPECS $TEST_TIME"
    fi
    TEST_TOTAL_TIME=30
    for TM in $TESTMODE; do
	let TEST_TOTAL_TIME+=$TEST_TIME
	let TEST_TOTAL_TIME+=60
    done

    # overall time:
    let N=$N_TESTS-$N_COMPILE
    let TOTAL_TIME=$STARTUP_TIME+$COMPILE_TIME
    let OVERALL_TIME=${TOTAL_TIME}*${N_COMPILE}
    let TOTAL_TIME=$STARTUP_TIME+$TEST_TOTAL_TIME
    let OVERALL_TIME+=${TOTAL_TIME}*${N}
    let OVERALL_MIN=$OVERALL_TIME/60
    let OVERALL_HOURS=$OVERALL_MIN/60
    let OVERALL_MIN=$OVERALL_MIN%60
    OVERALL_TIME=$(printf "%dh%02dmin" $OVERALL_HOURS $OVERALL_MIN)

    echo_log "run \"test $TEST_SPECS\" on batch file \"$BATCH_FILE\" with content:"
    sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | while read LINE; do echo_log "  $LINE"; done
    echo_log

    # read first line from configuration file:
    INDEX=1
    IFS=':' read DESCRIPTION LOAD_MODE NEW_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
    DESCRIPTION="$(echo $DESCRIPTION)"
    LOAD_MODE="$(echo $LOAD_MODE)"
    NEW_KERNEL_PARAM="$(echo $NEW_KERNEL_PARAM)"
    # in case of a config line, this sets the description of the actual kernel configuration:
    KERNEL_DESCR=""
    if test "x${LOAD_MODE}" = "xCONFIG" && test -z "$NEW_KERNEL_PARAM"; then
	KERNEL_DESCR="$DESCRIPTION"
	# read next line from configuration file:
	let INDEX+=1
	IFS=':' read DESCRIPTION LOAD_MODE NEW_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
	DESCRIPTION="$(echo $DESCRIPTION)"
	LOAD_MODE="$(echo $LOAD_MODE)"
	NEW_KERNEL_PARAM="$(echo $NEW_KERNEL_PARAM)"
    fi

    # report first batch entry:
    COMPILE=false
    if test "x${LOAD_MODE}" = "xCONFIG"; then
	COMPILE=true
	echo_log "Reboot into default kernel to compile kernel with \"$DESCRIPTION\" configuration."
    else
	COMPILE=false
	# assemble overall description:
	KD="${KERNEL_DESCR}"
	test -n "$KD" && test "${KD:-1:1}" != "-" && KD="${KD}-"
	KD="${KD}${KERNEL_PARAM_DESCR}"
	test -n "$KD" && test "${KD:-1:1}" != "-" && KD="${KD}-"
	# report next kernel parameter settings:
	echo_log "Reboot into first configuration: \"${KD}${DESCRIPTION}\" with kernel parameter \"$(echo $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEW_KERNEL_PARAM)\""
    fi

    # confirm batch testing:
    echo_log
    read -p "Do you want to proceed testing with $N_TESTS reboots (approx. ${OVERALL_TIME}) (Y/n)? " PROCEED
    if test "x$PROCEED" != "xn"; then
	echo_log
	cp $KERNEL_PATH/linux-${LINUX_KERNEL}-${KERNEL_SOURCE_NAME}/.config $KERNEL_CONFIG_BACKUP
	echo_log "Saved kernel configuration in \"$KERNEL_CONFIG_BACKUP\"."
	restore_test_batch
	# install crontab:
	MRK_DIR="$(cd "$(dirname "$0")" && pwd)"
	(crontab -l 2>/dev/null; echo "@reboot ${MRK_DIR}/${MAKE_RTAI_KERNEL} test batchscript > ${TEST_DIR}/testbatch.log") | crontab -
	echo_log "Installed crontab for automatic testing after reboot."
	echo_log "  Uninstall by calling"
	echo_log "  $ ./${MAKE_RTAI_KERNEL} restore testbatch"
	echo_kmsg "START TEST BATCH $BATCH_FILE"

	# set information for next test/compile:
	if test -f /boot/grub/grubenv; then
	    grub-editenv - set rtaitest_pwd="$PWD"
	    grub-editenv - set rtaitest_file="$BATCH_FILE"
	    grub-editenv - set rtaitest_index="$INDEX"
	    grub-editenv - set rtaitest_kernel_descr="$KERNEL_DESCR"
	    grub-editenv - set rtaitest_param_descr="$KERNEL_PARAM_DESCR"
	    grub-editenv - set rtaitest_time="$TEST_TOTAL_TIME"
	    grub-editenv - set rtaitest_specs="$TEST_SPECS"
	else
	    echo_kmsg "NEXT TEST BATCH |$PWD|$BATCH_FILE|$INDEX|$KERNEL_DESCR|$KERNEL_PARAM_DESCR|$TEST_TOTAL_TIME|$TEST_SPECS"
	fi
	if $COMPILE; then
	    reboot_kernel default
	else
	    reboot_kernel $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEW_KERNEL_PARAM
	fi
    else
	echo_log
	echo_log "Test batch aborted"
    fi

    exit 0
}

function test_abort {
    echo_kmsg "FAILED TO BUILD KERNEL"
    echo_log "FAILED TO BUILD KERNEL"
    # no further tests:
    echo_kmsg "ABORT TEST BATCH"
    echo_log "Abort test batch."
    # clean up:
    echo_log "Clean up test batch:"
    restore_test_batch > /dev/null
    restore_kernel_param
    reboot_unset_kernel
    echo_log
}

function test_batch_script {
    # run automatic testing of kernel parameter.
    # this function is called automatically after reboot from cron.

    PATH="$PATH:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

    # get paramter for current test/compile:
    if test -f /boot/grub/grubenv; then
	WORKING_DIR=$(grub-editenv - list | awk -F '=' '/^rtaitest_pwd=/ {print $2}')
	BATCH_FILE=$(grub-editenv - list | awk -F '=' '/^rtaitest_file=/ {print $2}')
	INDEX=$(grub-editenv - list | awk -F '=' '/^rtaitest_index=/ {print $2}')
	KERNEL_DESCR=$(grub-editenv - list | awk -F '=' '/^rtaitest_kernel_descr=/ {print $2}')
	BATCH_DESCR=$(grub-editenv - list | awk -F '=' '/^rtaitest_param_descr=/ {print $2}')
	TEST_TOTAL_TIME=$(grub-editenv - list | awk -F '=' '/^rtaitest_time=/ {print $2}')
	TEST_SPECS=$(grub-editenv - list | awk -F '=' '/^rtaitest_specs=/ {print $2}')
    else
	MF=/var/log/messages
	grep -q -a -F "NEXT TEST BATCH" $MF || MF=/var/log/messages.1
	IFS='|' read ID WORKING_DIR BATCH_FILE INDEX KERNEL_DESCR BATCH_DESCR TEST_TOTAL_TIME TEST_SPECS < <(grep -a -F "NEXT TEST BATCH" $MF | tail -n 1)
    fi
    KERNEL_DESCR="$(echo $KERNEL_DESCR)"
    BATCH_DESCR="$(echo $BATCH_DESCR)"

    # working directory:
    cd "$WORKING_DIR"

    N_TESTS=$(sed -e 's/ *#.*$//' $BATCH_FILE | grep -c ':.*:')

    # read configuration:
    source "${MAKE_RTAI_CONFIG}"

    # enable logs:
    LOG_FILE="${WORKING_DIR}/${MAKE_RTAI_KERNEL%.*}.log"
    echo_log
    echo_log "Automatically start test $INDEX of $N_TESTS in file \"$BATCH_FILE\"."
    echo_log

    # read current DESCRIPTION and LOAD_MODE from configuration file:
    IFS=':' read DESCRIPTION LOAD_MODE NEW_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
    DESCRIPTION="$(echo $DESCRIPTION)"
    LOAD_MODE="$(echo $LOAD_MODE)"
    NEW_KERNEL_PARAM="$(echo $NEW_KERNEL_PARAM)"
    # compile new kernel:
    COMPILE=false
    if test "x${LOAD_MODE}" = "xCONFIG"; then
	COMPILE=true
	KERNEL_DESCR="$DESCRIPTION"
    else
	# wait:
	sleep $STARTUP_TIME
    fi

    # next:
    let INDEX+=1

    if test "$INDEX" -gt "$N_TESTS"; then
	# no further tests:
	echo_kmsg "LAST TEST BATCH"
	echo_log "Final test"
	# clean up:
	echo_log "Clean up test batch:"
	restore_test_batch > /dev/null
	restore_kernel_param
	echo_log
    else
	# read next test:
	IFS=':' read DESCR LM NEXT_KERNEL_PARAM < <(sed -e 's/ *#.*$//' $BATCH_FILE | grep ':.*:' | sed -n -e ${INDEX}p)
	# set information for next test/compile:
	if test -f /boot/grub/grubenv; then
	    grub-editenv - set rtaitest_index="$INDEX"
	    grub-editenv - set rtaitest_kernel_descr="$KERNEL_DESCR"
	else
	    echo_kmsg "NEXT TEST BATCH |$WORKING_DIR|$BATCH_FILE|$INDEX|$KERNEL_DESCR|$BATCH_DESCR|$TEST_TOTAL_TIME|$TEST_SPECS"
	fi
	LM="$(echo $LM)"
	NEXT_KERNEL_PARAM="$(echo $NEXT_KERNEL_PARAM)"
	if test "x${LM}" != "xCONFIG"; then
	    echo_log "Prepare next reboot:"
	    setup_kernel_param $BATCH_KERNEL_PARAM $KERNEL_PARAM $NEXT_KERNEL_PARAM
	    reboot_set_kernel
	    echo_log
	fi
    fi

    # working directory:
    cd "$WORKING_DIR"

    if $COMPILE; then
	# compile new kernel:
	KERNEL_CONFIG="$NEW_KERNEL_PARAM"
	NEW_KERNEL_CONFIG=true
	if test -z "$KERNEL_CONFIG"; then
	    echo_log "Missing kernel configuration!"
	    echo_kmsg "Missing kernel configuration!"
	    test_abort
	fi
	echo_log "Compile new kernel:"
	echo_kmsg "START COMPILE NEW KERNEL"
	KERNEL_MENU=old
	reconfigure &> "${LOG_FILE}.tmp"
	if test "x$?" != "x0"; then
	    echo_kmsg "END COMPILE NEW KERNEL"
	    echo_log ""
	    echo_log "Detailed output of reconfigure:"
	    cat "${LOG_FILE}.tmp" >> "$LOG_FILE"
	    test_abort
	else
	    echo_kmsg "END COMPILE NEW KERNEL"
	fi
	rm "${LOG_FILE}.tmp"
    else
	# in case everything fails do a cold start:
	{ sleep $(( $TEST_TOTAL_TIME + 180 )); reboot_cmd cold; } &
	# at TEST_TOTAL_TIME seconds later reboot:
	{ sleep $TEST_TOTAL_TIME; reboot_cmd; } &

	# assemble description:
	test -n "$KERNEL_DESCR" && test "${KERNEL_DESCR:-1:1}" != "-" && KERNEL_DESCR="${KERNEL_DESCR}-"
	test -n "$BATCH_DESCR" && test "${BATCH_DESCR:-1:1}" != "-" && BATCH_DESCR="${BATCH_DESCR}-"

	# run tests:
	echo_log "test kernel ${KERNEL_DESCR}${BATCH_DESCR}${DESCRIPTION}:"
	test_kernel $TEST_SPECS $LOAD_MODE auto "${KERNEL_DESCR}${BATCH_DESCR}${DESCRIPTION}"
	echo_log
    fi

    if test "$INDEX" -gt "$N_TESTS"; then
	echo_kmsg "FINISHED TEST BATCH"
	echo_log "finished test batch"
    fi

    # reboot:
    sleep 1
    reboot_cmd
    sleep 300
    reboot_cmd cold

    exit 0
}

function restore_test_batch {
    if crontab -l | grep -q "${MAKE_RTAI_KERNEL}"; then
	echo_log "restore original crontab"
	if ! $DRYRUN; then
	    (crontab -l | grep -v "${MAKE_RTAI_KERNEL}") | crontab -
	fi
    fi
    if test -f /boot/grub/grubenv; then
	grub-editenv - unset rtaitest_pwd
	grub-editenv - unset rtaitest_file
	grub-editenv - unset rtaitest_index
	grub-editenv - unset rtaitest_kernel_descr
	grub-editenv - unset rtaitest_param_descr
	grub-editenv - unset rtaitest_time
	grub-editenv - unset rtaitest_specs
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
	# skip empty files: (better would be to list them as failed)
	test "$(wc -l $TEST | cut -d ' ' -f 1)" -lt 4 && continue
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
	echo "You need to specify existing files or a directory with test result files!"
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
	# skip noexisting files:
	test -f "$TEST" || continue
	# skip empty files: (better would be to list them as failed)
	test "$(wc -l $TEST | cut -d ' ' -f 1)" -lt 4 && continue
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
	    else
		echo_log "Failed to download newlib!"
		return 1
	    fi
	fi
    fi
}

function update_newlib {
    cd ${LOCAL_SRC_PATH}
    if test -d newlib/src/.git; then
	echo_log "update already downloaded newlib sources"
	cd newlib/src
	if git pull origin master; then
	    date +"%F %H:%M" > revision.txt
	    clean_newlib
	else
	    echo_log "Failed to update newlib!"
	    return 1
	fi
    elif ! test -f newlib/$NEWLIB_TAR; then
	rm -r newlib
	download_newlib || return 1
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
		if wget https://www.rtai.org/userfiles/downloads/RTAI/${RTAI_DIR}.tar.bz2; then
		    echo_log "unpack ${RTAI_DIR}.tar.bz2"
		    tar xof ${RTAI_DIR}.tar.bz2
		    # -o option because we are root and want the files to be root!
		else
		    echo_log "Failed to download RTAI from \"https://www.rtai.org/userfiles/downloads/RTAI/${RTAI_DIR}.tar.bz2\"!"
		    return 1
		fi
	    fi
	    if test "x$?" != "x0"; then
		echo_log "Failed to download RTAI!"
		return 1
	    else
		date +"%F %H:%M" > $RTAI_DIR/revision.txt
	    fi
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
		cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai update && date +"%F %H:%M" > revision.txt
	    fi
	elif test -d .git; then
	    echo_log "update already downloaded rtai sources"
	    if ! $DRYRUN; then
		git pull origin master && date +"%F %H:%M" > revision.txt
	    fi
	elif test -f ../${RTAI_DIR}.tar.bz2; then
	    cd -
	    echo_log "unpack ${RTAI_DIR}.tar.bz2"
	    tar xof ${RTAI_DIR}.tar.bz2
	    cd -
	fi
	cd -
	if test "x$?" != "x0"; then
	    echo_log "Failed to update RTAI!"
	    return 1
	fi
	echo_log "set soft link rtai -> $RTAI_DIR"
	if ! $DRYRUN; then
	    ln -sfn $RTAI_DIR rtai
	fi
    else
	download_rtai || return 1
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
	    if test "x$?" != "x0"; then
		echo_log "Failed to patch RTAI configuration!"
		return 1
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
		if test "x$?" != "x0"; then
		    echo_log "Failed to patch RTAI configuration for math support!"
		    return 1
		fi
	    fi
	    make -f makefile oldconfig
	    if test "x$?" != "x0"; then
		echo_log "Failed to clean RTAI configuration (make oldconfig)!"
		return 1
	    fi
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
	    if ! cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co $SHOWROOM_DIR; then
		echo_log "Failed to download showroom!"
		return 1
	    fi
	    date +"%F %H:%M" > $SHOWROOM_DIR/revision.txt
	fi
    fi
}

function update_showroom {
    cd ${LOCAL_SRC_PATH}
    if test -d $SHOWROOM_DIR; then
	echo_log "update already downloaded rtai-showroom sources"
	cd $SHOWROOM_DIR
	if ! cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai update; then
	    echo_log "Failed to update showroom!"
	    return 1
	fi
	date +"%F %H:%M" > revision.txt
	clean_showroom
    else
	download_showroom || return 1
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
	    if ! git clone https://github.com/Linux-Comedi/comedi.git; then
		echo_log "Failed to download comedi from \"git clone https://github.com/Linux-Comedi/comedi.git\"!"
		return 1
	    fi
	    date +"%F %H:%M" > comedi/revision.txt
	fi
    fi
}

function update_comedi {
    cd ${LOCAL_SRC_PATH}
    if test -d comedi; then
	echo_log "update already downloaded comedi sources"
	cd comedi
	if ! git pull origin master; then
	    echo_log "Failed to update comedi!"
	    return 1
	fi
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
	modprobe -r kcomedilib && echo_log "removed kcomedilib"
	for i in $(lsmod | grep "^comedi" | tail -n 1 | awk '{ m=$4; gsub(/,/,"\n",m); print m}' | tac); do
	    modprobe -r $i && echo_log "removed $i"
	done
	modprobe -r comedi && echo_log "removed comedi"
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
	    if test -f /var/log/messages; then
		echo_log "/var/log/messages is already enabled. No action required."
	    else
		echo_log "/etc/rsyslog.d/50-default.conf not found: cannot enable /var/log/messages."
	    fi
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

    # grub configuration file:
    if test -f /etc/default/grub.origmrk; then
	echo_log "Grub menu has already been configured."
    elif test -f /etc/default/grub; then
	cd /etc/default
	echo_log "Configure grub menu."
	if ! $DRYRUN; then
	    cp grub grub.origmrk
	    sed -e 's/GRUB_HIDDEN/#GRUB_HIDDEN/; s/GRUB_TIMEOUT=.*/GRUB_TIMEOUT=5/; /GRUB_CMDLINE_LINUX=/aexport GRUB_CMDLINE_RTAI=""' grub.origmrk > grub
	    ( echo; echo "GRUB_DISABLE_SUBMENU=y"; echo; echo "GRUB_DISABLE_RECOVERY=true" ) >> grub
	    RUN_UPDATE=true
	fi
    else
	echo_log "/etc/default/grub not found: cannot configure grub menu."
    fi

    # grub linux kernel script:
    if test -f /etc/grub.d/10_linux.origmrk; then
	echo_log "Grub linux script has already been configured."
    elif test -f /etc/grub.d/10_linux; then
	cd /etc/grub.d
	echo_log "Configure grub linux entries."
	if ! $DRYRUN; then
	    mv 10_linux 10_linux.origmrk
	    awk '{
                if ( /export linux_gfx/ ) {
                    ++level 
                }
                if ( level > 0 && /EOF/ ) {
                    print
                    print "\ncat << EOF\nload_env rtai_cmdline\nEOF"
                    next
                }
                if ( /initramfs=$/ ) {
                    print "  # check for RTAI kernel:"
                    print "  CMDLINE_RTAI=\"\""
                    print "  if grep -q \"CONFIG_IPIPE=y\" \"${config}\"; then"
                    print "      CMDLINE_RTAI=\"${GRUB_CMDLINE_RTAI} \\$rtai_cmdline\""
                    print "  fi"
                    print ""
                }
            }1' /etc/grub.d/10_linux.origmrk | \
	    sed -e '/SUPPORTED_INITS/{s/ systemd.*systemd//; s/ upstart.*upstart//;}' -e '/\${GRUB_CMDLINE_LINUX}.*\${GRUB_CMDLINE_LINUX_DEFAULT}/s/\(${GRUB_CMDLINE_LINUX}.*\)\(${GRUB_CMDLINE_LINUX_DEFAULT}\)/\1${CMDLINE_RTAI} \2/' > 11_linux
	    if ! grep -q GRUB_DISABLE_SUBMENU 11_linux > /dev/null; then
		sed -i -e '/if .*$in_submenu.*; then/,/fi$/s/^/#/' 11_linux
	    fi
	    chmod a-x 10_linux.origmrk
	    chmod a+x 11_linux
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
	echo_log "/boot/grub/grubenv not found: cannot enable reboot requests for normal user."
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
	    rm -f 11_linux
	    RUN_UPDATE=true
	fi
    fi
    if test -f /boot/grub/grubenv; then
	echo_log "Remove grub environment variables."
	if ! $DRYRUN; then
	    grub-editenv - unset rtai_cmdline
	    grub-editenv - unset next_entry
	    grub-editenv - unset rtaitest_file
	    grub-editenv - unset rtaitest_index
	    grub-editenv - unset rtaitest_kernel_descr
	    grub-editenv - unset rtaitest_param_descr
	    grub-editenv - unset rtaitest_time
	    grub-editenv - unset rtaitest_specs
	fi
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
    if getent group iocard > /dev/null; then
	echo_log "Delete group \"iocard\"."
	if ! $DRYRUN; then
	    delgroup iocard
	fi
    fi
}


###########################################################################
# actions:

function info_all {
    case $1 in

    grub ) print_grub env ;;

    settings )
	if [ -t 1 ]; then
	    print_settings
	    echo
	    echo "You may modify the settings by the respective command line options (check \$ ${MAKE_RTAI_KERNEL} help),"
	    echo "by setting them in the configuration file \"${MAKE_RTAI_CONFIG}\""
	    echo "(create configuration file by \$ ${MAKE_RTAI_KERNEL} info settings > ${MAKE_RTAI_CONFIG}), or"
	    echo "by editing the variables directly in the ${MAKE_RTAI_KERNEL} script."
	else
	    print_config
	fi
	;;

    setup ) print_setup ;;

    log ) print_log ;;

    configs )
	shift
	print_kernel_configs $@
	;;

    menu ) menu_kernel ;;

    kernel ) print_kernel ;;

    cpu|cpus ) print_cpus ;;

    * )
	print_full_info $@ 
	test "x$1" = "xrtai" && rm -f "$LOG_FILE"
	;;

    esac
}

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
	restore_kernel_param
	restore_grub
	restore_comedi
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

    SECONDS=0

    if ! install_packages; then
	return 1
    fi

    uninstall_kernel
    ${MAKE_NEWLIB} && uninstall_newlib
    ${MAKE_RTAI} && uninstall_rtai
    ${MAKE_COMEDI} && uninstall_comedi

    ${MAKE_RTAI} && { download_rtai || return 1; }
    ${MAKE_NEWLIB} && { download_newlib || MAKE_NEWLIB=false; }
    ${MAKE_COMEDI} && { download_comedi || MAKE_COMEDI=false; }
    download_kernel || return 1

    unpack_kernel && patch_kernel && build_kernel || return 1

    ${MAKE_NEWLIB} && { build_newlib || MAKE_NEWLIB=false; }
    ${MAKE_RTAI} && { build_rtai || return 1; }
    ${MAKE_COMEDI} && { build_comedi || MAKE_COMEDI=false; }

    SECS=$SECONDS
    let MIN=${SECS}/60
    let SEC=${SECS}%60

    echo_log
    echo_log "Done!"
    echo_log "Full build took ${SECS} seconds ($(printf "%02d:%02d" $MIN $SEC))."
    echo_log "Please reboot into the ${KERNEL_NAME} kernel by executing"
    echo_log "$ ./${MAKE_RTAI_KERNEL} reboot"
    echo_log
}

function reconfigure {
    RECONFIGURE_KERNEL=true
    check_root

    SECONDS=0

    uninstall_kernel
    unpack_kernel && patch_kernel && build_kernel || return 1

    ${MAKE_NEWLIB} && { build_newlib || MAKE_NEWLIB=false; }

    ${MAKE_RTAI} && uninstall_rtai
    ${MAKE_RTAI} && { build_rtai || return 1; }

    ${MAKE_COMEDI} && uninstall_comedi
    ${MAKE_COMEDI} && build_comedi

    SECS=$SECONDS
    let MIN=${SECS}/60
    let SEC=${SECS}%60

    echo_log
    echo_log "Done!"
    echo_log "Build took ${SECS} seconds ($(printf "%02d:%02d" $MIN $SEC))."
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
	unpack_kernel && patch_kernel && build_kernel || return 1
	${MAKE_NEWLIB} && { build_newlib || MAKE_NEWLIB=false; }
	${MAKE_RTAI} && { build_rtai || return 1; }
	${MAKE_COMEDI} && build_comedi
    else
	for TARGET; do
	    case $TARGET in
		kernel ) 
		    unpack_kernel && patch_kernel && build_kernel || return 1
		    ${MAKE_NEWLIB} && { build_newlib || MAKE_NEWLIB=false; }
		    ${MAKE_RTAI} && { build_rtai || return 1; }
		    ${MAKE_COMEDI} && build_comedi
		    ;;
		newlib )
		    build_newlib || return 1
		    ${MAKE_RTAI} && { build_rtai || return 1; }
		    ${MAKE_COMEDI} && build_comedi
		    ;;
		rtai ) 
		    build_rtai || return 1
		    ${MAKE_COMEDI} && build_comedi
		    ;;
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

# read in configuration:
if test -f "$MAKE_RTAI_CONFIG"; then
    source "$MAKE_RTAI_CONFIG"
fi

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
		shift
		NEW_KERNEL_CONFIG=true
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

if test "x$1" != "xhelp" && test "x$1" != "xversion" && test "x$1" != "xinfo" && test "x$1" != "xreport" && test "x$1" != "xconfig" && ! ( test "x$1" = "xtest" && test "x$2" = "xbatchscript" ); then
    rm -f "$LOG_FILE"
fi

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

    info ) info_all $@
	exit 0
	;;

    config ) 
	if test -f "${MAKE_RTAI_CONFIG}"; then
	    echo "Configuration file \"${MAKE_RTAI_CONFIG}\" already exists!"
	else
	    print_config > "${MAKE_RTAI_CONFIG}"
	    echo "Wrote configuration to file \"${MAKE_RTAI_CONFIG}\"".
	fi
	exit 0
	;;

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
    prepare ) prepare_kernel_configs $@ ;;
    build ) build_all $@ ;;
    buildplain ) buildplain_kernel $@ ;;
    install ) install_all $@ ;;
    clean ) clean_all $@ ;;
    uninstall ) uninstall_all $@ ;;
    remove ) remove_all $@ ;;
    reboot ) reboot_kernel $@ ;;

    reconfigure ) reconfigure ;;

    * ) if test -n "$ACTION"; then
	    echo "unknown action \"$ACTION\""
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
