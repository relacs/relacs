#! /usr/bin/env bash
VERSION=0.9.6

TIMESTAMP_START=`date +%s`
PKGDIR=relacs-${VERSION}
PKGFILE=${PKGDIR}.tar.gz
ORIGFILE=relacs_${VERSION}.orig.tar.gz
CHANGESFILE=relacs_${VERSION}-1_i386.changes

# Ensure we work with a fresh working package
if true ; then
    rm "${PKGFILE}"

    # Prevent mis-configure trouble
    ./bootstrap.sh || exit 1
    rm config.status
    find . -name '*.la' -print -delete

    ./configure --prefix=/usr || exit
    make dist || exit 1
fi

# Put everything in place
cd .. || exit 1
rm "${ORIGFILE}"
cp "trunk/${PKGFILE}" "${ORIGFILE}" || exit 1
tar xf "${ORIGFILE}" || exit 
cd "${PKGDIR}" || exit 1 
if [ ! -d debian ]; then
	svn co https://relacs.svn.sourceforge.net/svnroot/relacs/relacs/trunk/debian debian || exit 1
else
	svn up debian || exit 1
fi

# Prevent mis-configure trouble
rm config.status
find . -name '*.la' -print -delete

# Create package
dpkg-buildpackage -d -us -uc -rfakeroot -nc || exit 1

# Print warnings if any
echo BEGIN  lintian output
lintian "../${CHANGESFILE}" -i || exit 1
echo END    lintian output

# Go home
cd ../trunk

# Stats
TIMESTAMP_STOP=`date +%s`
TIMESTAMP_DIFF=$((TIMESTAMP_STOP - TIMESTAMP_START))
DURATION_MINUTES=$((TIMESTAMP_DIFF / 60))
DURATION_SECONDS=$((TIMESTAMP_DIFF % 60))

printf 'DONE, took %02d:%02d\n' ${DURATION_MINUTES} ${DURATION_SECONDS}
exit 0

