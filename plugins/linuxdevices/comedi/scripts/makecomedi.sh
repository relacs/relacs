#!/bin/bash

cd /usr/src

# install necessary packages:
sudo apt-get install dpkg-dev

# install kernel headers:
sudo apt-get install linux-headers-`uname -r`

# install kernel sources:
SRCPACKAGE=linux-source-`uname -r`
echo "download $SRCPACKAGE ..."
if ! sudo apt-get source $SRCPACKAGE; then
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
    if ! sudo apt-get source $SRCPACKAGE; then
	echo "FAILED TO INSTALL KERNEL SOURCE PACKAGE!"
	exit 1
    fi
fi
echo "unpack $SRCPACKAGE ..."
tar xf $SRCPACKAGE.tar.bz2
ln -sfn $SRCPACKAGE linux

# prepare kernel sources:
cd linux
cp /boot/config-`uname -r` .config
cp ../linux-headers-`uname -r`/Module.symvers .
make silentoldconfig
make prepare
make scripts

# install comedi:
# install all required packages:
apt-get -y install gcc g++ git autoconf automake libtool bison flex libgsl0-dev libboost-program-options-dev

# download comedi sources:
cd /usr/local/src
git clone https://github.com/Linux-Comedi/comedi.git
#git clone https://github.com/Linux-Comedi/comedilib.git
#git clone https://github.com/Linux-Comedi/comedi_calibrate.git

# remove comedi from the kernel's staging directory:
rm -r /lib/modules/`uname -r`/kernel/drivers/staging/comedi

# comedi:
cd /usr/local/src/comedi
./autogen.sh
./configure
make -j$(grep -c "^processor" /proc/cpuinfo)
make install
depmod -a
cp include/linux/comedi.h /usr/include/linux/
cp include/linux/comedilib.h /usr/include/linux/

exit 0

# comedilib:
cd /usr/local/src/comedilib
./autogen.sh
./configure --prefix=/usr --sysconfdir=/etc
make -j$(grep -c "^processor" /proc/cpuinfo)
make install

# comedi_calibrate:
cd /usr/local/src/comedi_calibrate
autoreconf -v -i
./configure --prefix=/usr --sysconfdir=/etc
make
make install

# setup udev permissions:
groupadd --system iocard
echo 'KERNEL=="comedi*", MODE="0660", GROUP="iocard"' > /etc/udev/rules.d/95-comedi.rules
udevadm trigger
usermod $USER -a -G iocard   # or: adduser $USER iocard



