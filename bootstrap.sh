#! /bin/bash
CUR=0
COUNT=4

step() {
	PERC=$((100 * CUR / COUNT))
	printf "[%3i%%] %s" ${PERC} "$1"
	echo
	CUR=$((CUR + 1))
}

## Aclocal
ACLOCAL_VERSION=`aclocal --version 2> /dev/null | head -1 | grep -o '[^ ]*$'`
if test x$ACLOCAL_VERSION = x ; then
    echo "! error: aclocal seems not to be installed on your system."
    echo "Please install the \"automake\" package (version 1.10 or higher)"
    echo "and run ./bootstrap.sh again."
    exit 1
fi
step "aclocal     ${ACLOCAL_VERSION}"
LIBTOOL_M4=$(dirname $(dirname $(which libtool)))/share/libtool/libtool.m4
if [ -e ${LIBTOOL_M4} ]; then
	cp "${LIBTOOL_M4}" acinclude.m4 || exit 1
fi
AMINCS=$(grep ACLOCAL_AMFLAGS Makefile.am)
aclocal ${AMINCS#*=}  || exit 1

## Libtoolize
LIBTOOLIZE=libtoolize
WHITESPACE=''
if glibtoolize --version &>/dev/null ; then
	LIBTOOLIZE=glibtoolize
	WHITESPACE=' '
fi
LIBTOOLIZE_VERSION=`${LIBTOOLIZE} --version 2> /dev/null | head -1 | grep -o '[^ ]*$'`
if test x$LIBTOOLIZE_VERSION = x ; then
    echo
    echo "! error: libtoolize seems not to be installed on your system."
    echo "Please install the \"libtool\" package and run ./bootstrap.sh again."
    exit 1
fi
step "${LIBTOOLIZE}${WHITESPACE}${LIBTOOLIZE_VERSION}"
${LIBTOOLIZE} --copy --force >/dev/null || exit 1

## Autoconf
AUTOCONF_VERSION=`autoconf --version 2> /dev/null | head -1 | grep -o '[^ ]*$'`
if test x$AUTOCONF_VERSION = x ; then
    echo
    echo "! error: autoconf seems not to be installed on your system."
    echo "Please install the \"autoconf\" package and run ./bootstrap.sh again."
    exit 1
fi
step "autoconf    ${AUTOCONF_VERSION}"
autoconf || exit 1

## Automake
AUTOMAKE_VERSION=`automake --version 2> /dev/null | head -1 | grep -o '[^ ]*$'`
if test x$AUTOMAKE_VERSION = x ; then
    echo
    echo "! error: automake seems not to be installed on your system."
    echo "Please install the \"automake\" package (version 1.10 or higher)"
    echo "and run ./bootstrap.sh again."
    exit 1
fi
step "automake    ${AUTOMAKE_VERSION}"
automake --add-missing --copy || exit 1

step "."

## Finished:
echo
echo "Successfully generated the ./configure script."
echo "Continue with calling"
echo "./configure"

exit 0

