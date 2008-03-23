#! /bin/bash
CUR=0
COUNT=5

step() {
	PERC=$((100 * CUR / COUNT))
	printf "[%3i%%] %s" ${PERC} "$1"
	echo
	CUR=$((CUR + 1))
}

## Aclocal
ACLOCAL_VERSION=`aclocal --version | head -1 | grep -o '[^ ]*$'`
step "aclocal     ${ACLOCAL_VERSION}"
LIBTOOL_M4=$(dirname $(dirname $(which libtool)))/share/libtool/libtool.m4
if [ -e ${LIBTOOL_M4} ]; then
	cp "${LIBTOOL_M4}" acinclude.m4 || exit 1
fi
aclocal -I numerics/m4 -I hardware/m4 -I m4 || exit 1

## Libtoolize
LIBTOOLIZE=libtoolize
WHITESPACE='  '
if glibtoolize --version &>/dev/null ; then
	LIBTOOLIZE=glibtoolize
	WHITESPACE=' '
fi
LIBTOOLIZE_VERSION=`${LIBTOOLIZE} --version | head -1 | grep -o '[^ ]*$'`
step "${LIBTOOLIZE}${WHITESPACE}${LIBTOOLIZE_VERSION}"
${LIBTOOLIZE} --copy --force >/dev/null || exit 1

## Autoconf
AUTOCONF_VERSION=`autoconf --version | head -1 | grep -o '[^ ]*$'`
step "autoconf    ${AUTOCONF_VERSION}"
touch build_config_dump.in || exit 1 # Solves chicken-and-egg problem
autoconf || exit 1

## Dump build config
step "build_config_dump.in"
awk -F"'" 'function pp(s) { return sprintf("%s = [@%s@]", s, s)}; start { print pp($1); \
    if (NF > 1) exit }; $1 == "ac_subst_vars=" {start=1; print pp($2)}' configure \
    | sort -f > build_config_dump.in || exit 1

## Automake
AUTOMAKE_VERSION=`automake --version | head -1 | grep -o '[^ ]*$'`
step "automake    ${AUTOMAKE_VERSION}"
automake --add-missing --copy || exit 1

step "."
exit 0

