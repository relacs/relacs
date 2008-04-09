#! /bin/sh
VERSION=0.9.6

PKGDIR=relacs-${VERSION}
PKGFILE=${PKGDIR}.tar.gz
ORIGFILE=../relacs_${VERSION}.orig.tar.gz
CHANGESFILE=../relacs_${VERSION}.changes

if [ ! -f ${PKGFILE} ]; then
    ./bootstrap.sh || exit 1
    make dist || exit 1
fi

cp ${PKGFILE} ${ORIGFILE} || exit 1

dpkg-buildpackage -us -uc -rfakeroot -nc || exit 1
lintian "${CHANGESFILE}" -i || exit 1

echo DONE.
exit 0

