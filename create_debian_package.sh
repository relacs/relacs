#! /bin/sh
VERSION=0.9.6

PWD_BACKUP=$PWD

PKGDIR=relacs-${VERSION}
PKGFILE=${PKGDIR}.tar.gz
ORIGFILE=../relacs_${VERSION}.orig.tar.gz

if [ ! -f ${PKGFILE} ]; then
    ./bootstrap.sh || exit 1
    make dist || exit 1
fi

if [ ! -f ${ORIGFILE} ]; then
    cp ${PKGFILE} ${ORIGFILE} || exit 1
fi

dpkg-buildpackage -us -uc -rfakeroot -nc || exit 1

exit 0

