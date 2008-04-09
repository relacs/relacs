#! /usr/bin/env bash

if test "x$1" = "x--help"; then
  echo "Usage:"
  echo ""
  echo "release.sh <install-path> <configure-options>"
  echo ""
  echo "Calls ./bootstrap.sh && ./configure && make && make check &&"
  echo "make install && make distcheck for the whole relacs project"
  echo "and the individual relacs libraries."
  echo "You must supply an absolute <install-path> as the first argument."
  echo "All following arguments are supplied as further options to configure."
  echo ""
  exit 0
fi

INSTALLPATH="$1"
shift
if [ x${INSTALLPATH} = x ]; then
  echo "You need to specify an absolute installation path."
  exit 1
fi
CONFIGUREPREFIX="--prefix=$INSTALLPATH"

# further options for configure (without prefix):
CONFIGUREFLAGS="$@"

if [ x${MAKE} = x ]; then
    MAKE=make
fi

# minimal clean up:
echo "Clean up *.la files:"
find . -name '*.la' -print -delete
echo ""

# configure and check everything:
echo "Check relacs:"
./bootstrap.sh || exit 1
./configure $CONFIGUREPREFIX $CONFIGUREFLAGS # Also creates configure.ac files of libraries
${MAKE} || exit 1
${MAKE} check || exit 1
${MAKE} install || exit 1
${MAKE} distcheck || exit 1
echo ""
echo "Successfully checked relacs"
echo ""


# configure and check everything:
export CPPFLAGS="-I${INSTALLPATH}/include"
export LDFLAGS="-L${INSTALLPATH}/lib"
export DISTCHECK_CONFIGURE_FLAGS="$CONFIGUREFLAGS"
for i in numerics daq options datafile widgets config mplot; do
    echo "Check $i library:"
    cd $i || exit 1
    find . -name '*.la' -print -delete
    ./bootstrap.sh || exit 1
    ./configure $CONFIGUREPREFIX $CONFIGUREFLAGS || exit 1
    ${MAKE} || exit 1
    ${MAKE} check || exit 1
    ${MAKE} install || exit 1
    ${MAKE} distcheck || exit 1
    cd - || exit 1
    echo ""
    echo "Successfully checked $i library"
    echo ""
done

echo "Successfully checked relacs and all its libraries!"
echo "You can go ahead and make a release (using releaseforge) :-)"
echo ""
exit 0

