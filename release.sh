#! /usr/bin/env bash

if test "x$1" = "x--help"; then
  echo "Usage:"
  echo ""
  echo "release.sh [-l] [-d] [--prefix=]<install-path> <configure-options>"
  echo ""
  echo "Calls ./bootstrap.sh && ./configure && make distcheck &&"
  echo "make && make check && make install && make installcheck"
  echo "for the whole relacs project and the individual relacs libraries"
  echo "(if the -l option is supplied)."
  echo "You must supply an absolute <install-path> as the first argument."
  echo "All following arguments are supplied as further options to configure."
  echo ""
  echo "-d: make distcheck only"
  echo "-l: check the relacs library packages as well"
  exit 0
fi

CHECKLIBS=no
if test "x$1" = "x-l"; then
  CHECKLIBS=yes
  shift
fi

DISTCHECKONLY=no
if test "x$1" = "x-d"; then
  DISTCHECKONLY=yes
  shift
fi

INSTALLPATH="${1#--prefix=}"
shift
if test "x${INSTALLPATH}" = x; then
  echo "You need to specify an installation path."
  exit 1
fi
INSTALLPATH=$(cd "$INSTALLPATH"; pwd)
CONFIGUREPREFIX="--prefix=$INSTALLPATH"

# further options for configure (without prefix):
CONFIGUREFLAGS="$@"
export DISTCHECK_CONFIGURE_FLAGS="$CONFIGUREFLAGS"

if test "x${MAKE}" = x; then
    MAKE=make
fi

# minimal clean up:
echo "Clean up *.la files:"
find . -name '*.la' -print -delete
echo ""

# configure and check everything:
echo "Check relacs:"
./bootstrap.sh || exit 1
echo "./configure $CONFIGUREPREFIX $CONFIGUREFLAGS"
./configure $CONFIGUREPREFIX $CONFIGUREFLAGS # Also creates configure.ac files of libraries
make uninstall
${MAKE} distcheck || exit 1
echo ""
if test $DISTCHECKONLY = no; then
  ${MAKE} || exit 1
  ${MAKE} check || exit 1
  ${MAKE} install || exit 1
  ${MAKE} installcheck || exit 1
fi
echo ""
echo "Successfully checked RELACS!"
echo "Upload the release to sourceforge using"
echo "  rsync -avP -e ssh relacs-0.9.7.tar.gz janbenda,relacs@frs.sourceforge.net:/home/frs/project/r/re/relacs/relacs/"
echo ""

# configure and check everything for the libraries:
if test $CHECKLIBS = yes; then
  export CPPFLAGS="-I${INSTALLPATH}/include"
  export LDFLAGS="-L${INSTALLPATH}/lib"
  for i in numerics daq options datafile widgets config plot; do
      echo "Check $i library:"
      cd $i || exit 1
      find . -name '*.la' -print -delete
      ./bootstrap.sh || exit 1
      ./configure $CONFIGUREPREFIX $CONFIGUREFLAGS || exit 1
      if test $DISTCHECKONLY = no; then
        ${MAKE} || exit 1
        ${MAKE} check || exit 1
        ${MAKE} install || exit 1
	${MAKE} installcheck || exit 1
        ${MAKE} distcheck || exit 1
      fi
      cd - || exit 1
      echo ""
      echo "Successfully checked $i library"
      echo ""
  done

  echo "Successfully checked relacs and all its libraries!"
  echo "You can go ahead and make a release :-)"
  echo "Upload the release to sourceforge using"
  echo "  rsync -avP -e ssh relacs-0.9.7.tar.gz janbenda,relacs@frs.sourceforge.net:/home/frs/project/r/re/relacs/relacs/"
  echo ""

fi

exit 0

