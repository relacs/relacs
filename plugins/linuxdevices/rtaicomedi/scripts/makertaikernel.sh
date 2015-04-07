#!/bin/bash

LINUX_KERNEL="3.14.17"
RTAI_PATCH="x86/patches/hal-linux-3.14.17-x86-6x.patch"
KERNEL_PATH=/data/src
RECONFIGURE_KERNEL=no

# we need to be root:
if test "x$(id -u)" != "x0"; then
    echo "you need to be root to run this script"
    exit 1
fi

###########################################################################
# clean all source trees:
if test "x$1" == "xclean"; then
    # clean all targets
    # kernel source tree:
    cd $KERNEL_PATH
    if test -d linux-${LINUX_KERNEL}-rtai; then
	rm -r linux-${LINUX_KERNEL}-rtai
    fi
    # rtai:
    cd /usr/local/src
    if test -d magma; then
	cd magma
	make clean
    fi
    # newlib:
    cd /usr/local/src
    if test -d newlib; then
	rm -r install/*
	cd newlib/src/newlib
	make clean
    fi
    # comedi:
    cd /usr/local/src
    if test -d comedi; then
	cd comedi
	make clean
    fi
    exit 0

###########################################################################
# remove installed kernel and rtai:
elif test "x$1" == "xremove"; then
    if test $(uname -r) = ${LINUX_KERNEL}-rtai; then
	echo "Cannot remove a running kernel!"
	echo "First boot into a different kernel."
	exit 1
    fi
    # rtai:
    if test -d /usr/realtime; then
	rm -r /usr/realtime
    fi
    # kernel:
    apt-get remove linux-image-${LINUX_KERNEL}-rtai
    cd /usr/src/linux
    if test -f ../linux-image-${LINUX_KERNEL}-rtai_1.0_amd64.deb; then
	rm ../linux-image-${LINUX_KERNEL}-rtai_1.0_amd64.deb
    fi
    exit 0

fi


###########################################################################
# build rtai-patched kernel:

# required packages:
apt-get -y install make gcc libncurses-dev zlib1g-dev kernel-package g++ git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev

# download rtai:
cd /usr/local/src
if test -d magma; then
    echo "keep already downloaded rtai sources"
    cd magma
    make distclean
    cd -
else
    echo "download rtai"
    cvs -d:pserver:anonymous@cvs.gna.org:/cvs/rtai co magma
fi
ln -sfn magma rtai

# download newlib library:
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

# download comedi:
cd /usr/local/src
if test -d comedi; then
    echo "keep already downloaded comedi sources"
else
    echo "download comedi"
    git clone git://comedi.org/git/comedi/comedi.git
fi

# download linux kernel:
cd $KERNEL_PATH
if test -f linux-$LINUX_KERNEL.tar.xz; then
    echo "keep already downloaded linux kernel archive"
else
    echo "download linux kernel version $LINUX_KERNEL"
    wget https://www.kernel.org/pub/linux/kernel/v3.x/linux-$LINUX_KERNEL.tar.xz
fi

NEW_RTAI_KERNEL=no
if test -d linux-${LINUX_KERNEL}-rtai; then
    echo "keep already existing linux-${LINUX_KERNEL}-rtai directory."
else
    # unpack:
    echo "unpack kernel sources from archive"
    tar xf linux-$LINUX_KERNEL.tar.xz
    mv linux-$LINUX_KERNEL linux-${LINUX_KERNEL}-rtai
    NEW_RTAI_KERNEL=yes
fi

# standard softlink to kernel:
cd /usr/src
ln -sfn $KERNEL_PATH/linux-${LINUX_KERNEL}-rtai linux
cd /usr/src/linux

# patch the kernel:
if test "$NEW_RTAI_KERNEL" = "yes"; then
    echo "apply rtai patch to kernel sources"
    patch -p1 < /usr/local/src/rtai/base/arch/$RTAI_PATCH
fi

if test "$NEW_RTAI_KERNEL" = "yes" -o "$RECONFIGURE_KERNEL" = "yes"; then
    # clean:
    make mrproper
    make-kpkg clean

    # kernel configuration:
    cp /boot/config-`uname -r` .config
    make silentoldconfig
    make localmodconfig

    # build the kernel:
    export CONCURRENCY_LEVEL=$(grep -c "^processor" /proc/cpuinfo)
    make-kpkg --initrd --append-to-version -rtai --revision 1.0 --config menuconfig kernel-image

    # install:
    if test -f ../linux-image-${LINUX_KERNEL}-rtai_1.0_amd64.deb; then
	dpkg -i ../linux-image-${LINUX_KERNEL}-rtai_1.0_amd64.deb
    else
	echo "failed to build kernel"
	exit 1
    fi
else
    echo "keep already compiled linux ${LINUX_KERNEL}-rtai kernel."
fi

# newlib library:
cd /usr/local/src/newlib
if test -f install/lib/libm.a; then
    echo "keep already installed newlib library"
else
    cd src/newlib
    ./configure --prefix=/usr/local/src/newlib/install --disable-shared CFLAGS="-O2 -mcmodel=kernel"
    make
    make install
fi

# rtai modules:
cd /usr/local/src/rtai
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
#make menuconfig
make
make install

# comedi:
if test -f /lib/modules/${LINUX_KERNEL}-rtai/comedi/comedi.ko; then
    echo "keep already installed comedi modules"
else
    cd /usr/local/src/comedi
    cp /usr/realtime/modules/Module.symvers comedi/
    ./autogen.sh
    PATH="$PATH:/usr/realtime/bin"
    ./configure --with-linuxdir=/usr/src/linux --with-rtaidir=/usr/realtime
    make -j$(grep -c "^processor" /proc/cpuinfo)
    make install
    depmod -a
    cp /usr/local/src/comedi/comedi/Module.symvers /lib/modules/${LINUX_KERNEL}-rtai/comedi/
    cp /usr/local/src/comedi/include/linux/comedi.h /usr/include/linux/
    cp /usr/local/src/comedi/include/linux/comedilib.h /usr/include/linux/
fi

echo
echo "Done!"
echo "Please reboot into the ${LINUX_KERNEL}-rtai kernel"

