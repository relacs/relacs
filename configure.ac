# Process this file with autoconf to produce a configure script.

AC_INIT(relacs, 0.9.6)
AM_INIT_AUTOMAKE([1.9 foreign subdir-objects dist-zip dist-bzip2])

AC_PROG_CXX
AC_PROG_LIBTOOL

AC_LANG(C++)

AM_PROG_CC_C_O

# Static substitutions
PLUGINS_BASENAME=relacs/plugins
AC_SUBST(PLUGINS_BASENAME)

RELACSDATE=`date '+%m/%d/%y'`
AC_SUBST(RELACSDATE)

RELACSVERSION="${VERSION} (${RELACSDATE})"
AC_SUBST(RELACSVERSION)



# Backup before any additions
CLEAN_LDFLAGS=${LDFLAGS}
CLEAN_CPPFLAGS=${CPPFLAGS}
CLEAN_LIBS=${LIBS}



#################################################
## GNU make
#################################################
NON_GNU_MAKE="make is not GNU make. Please fix this."
if ! make --version | grep 'GNU' &>/dev/null ; then
	AC_MSG_ERROR(${NON_GNU_MAKE})
fi



#################################################
## Qt3
#################################################
QT3_LDFLAGS=`pkg-config --libs qt-mt`
if test "x${QT3_LDFLAGS}" = x ; then
    QT3_LDFLAGS=-lqt-mt
fi
QT3_CPPFLAGS=`pkg-config --cflags qt-mt`
if test "x${QT3_CPPFLAGS}" = x ; then
    QT3_CPPFLAGS=-DQT_THREAD_SUPPORT
fi

LDFLAGS="${QT3_LDFLAGS} ${LDFLAGS}"
CPPFLAGS="${QT3_CPPFLAGS} ${CPPFLAGS}"

EXTRA_MOC_LOCATION=
AC_ARG_WITH(qt3, [
Extra Options:
  --with-qt3=DIR          override Qt3 path
                          ("/lib" and "/include" is appended], [
	QT3_ERROR="No path given for option --with-qt3"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_LDFLAGS="-L${withval} ${QT3_LDFLAGS}"
		LDFLAGS="-L${withval} ${LDFLAGS}"
		QT3_CPPFLAGS="-I${withval} ${QT3_CPPFLAGS}"
		CPPFLAGS="-I${withval} ${CPPFLAGS}"
		EXTRA_MOC_LOCATION="${withval}/bin"
	else
		AC_MSG_ERROR(${QT3_ERROR})
	fi
], [])

AC_ARG_WITH(qt3-inc, [  --with-qt3-inc=DIR      override Qt3 include path], [
	QT3_INC_ERROR="No path given for option --with-qt3-inc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_CPPFLAGS="-I${withval} ${QT3_CPPFLAGS}"
		CPPFLAGS="-I${withval} ${CPPFLAGS}"
	else
		AC_MSG_ERROR(${QT3_INC_ERROR})
	fi
], [])

AC_ARG_WITH(qt3-lib, [  --with-qt3-lib=DIR      override Qt3 library path], [
	QT3_LIB_ERROR="No path given for option --with-qt3-lib"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_LDFLAGS="-L${withval} ${QT3_LDFLAGS}"
		LDFLAGS="-L${withval} ${LDFLAGS}"
	else
		AC_MSG_ERROR(${QT3_LIB_ERROR})
	fi
], [])

QT3_MISSING="Please install Qt3.
   On a Debian-based system enter 'sudo apt-get install libqt3-mt-dev'."
AC_CHECK_HEADERS(qvbox.h qconfig.h,, AC_MSG_ERROR(${QT3_MISSING}))
AC_CHECK_LIB(qt-mt, main,, AC_MSG_ERROR(${QT3_MISSING}), ${QT3_LDFLAGS})

QT3_WRONG_VESION="Qt3 is required, you have another version."
AC_COMPILE_IFELSE([
#include <qvbox.h>
#if !defined(QT_VERSION) || (QT_VERSION < 0x030300) || (QT_VERSION >= 0x040000)
# error Wrong Qt version
#endif
],,AC_MSG_ERROR(${QT3_WRONG_VESION}))

AC_SUBST(QT3_LDFLAGS)
AC_SUBST(QT3_CPPFLAGS)

# Restore
LDFLAGS=${CLEAN_LDFLAGS}
CPPFLAGS=${CLEAN_CPPFLAGS}
LIBS=${CLEAN_LIBS}



#################################################
## MOC
#################################################
FORCED_MOC=
AC_ARG_WITH(moc, [  --with-moc=CMD          override moc command], [
	QT3_MOC_ERROR="No path given for option --with-moc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		FORCED_MOC=${withval}
	else
		AC_MSG_ERROR(${QT3_MOC_ERROR})
	fi
], [])

QT3_MOC_VERSION_ERROR="\$MOC is not pointing to Moc of Qt3"
if test "x${FORCED_MOC}" != x ; then
    # Moc command passed to configure
    MOC=${FORCED_MOC}
    if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
	    AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
    fi
else
    if test "x${EXTRA_MOC_LOCATION}" != x ; then
        # Moc from Qt3 directory
        MOC="${EXTRA_MOC_LOCATION}/moc"
        if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
            # Moc-qt3 from path
            MOC=moc-qt3
            if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
                # Moc from path
                MOC=moc
                if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
	                AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
                fi
            fi
        fi
    else
        # Moc-qt3 from path
        MOC=moc-qt3
        if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
            # Moc from path
            MOC=moc
            if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
                AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
            fi
        fi
    fi
fi

AC_SUBST(MOC)



#################################################
## Comedi
#################################################
AC_RELACS_CHECK_COMEDI



#################################################
## GSL
#################################################
DOXYGEN_PREDEF="RELACSDATA"
AC_RELACS_CHECK_GSL()



#################################################
## libc
#################################################
LIBDL_MISSING="Please install libdl, often shipped with the C Runtime Library.
   On a Debian-based system enter 'sudo apt-get install libc6-dev'."
AC_CHECK_LIB(dl, main,, AC_MSG_ERROR(${LIBDL_MISSING}))



#################################################
## libstdc++
#################################################
LIBSTDCPP_MISSING="Please install the C++ Standard Library (libstdc++).
   On a Debian-based system enter 'sudo apt-get install libstdc++6-4.2-dev'."
AC_CHECK_LIB(stdc++, main,, AC_MSG_ERROR(${LIBSTDCPP_MISSING}))



#################################################
## Link tests
#################################################
RELACS_LINKTESTS_ENABLED=yes
AC_ARG_ENABLE(linktests, [
  --disable-linktests    disable static plugin link tests], [
	if test ${enableval} = no ; then
		RELACS_LINKTESTS_ENABLED=no
	fi
], [])

LINKTESTS_DISABLED="Link tests disabled"
LINKTESTS_ENABLED="Link tests enabled (configure with --disable-linktests to disable)"

if test ${RELACS_LINKTESTS_ENABLED} = yes ; then
	AC_MSG_NOTICE(${LINKTESTS_ENABLED})
else
	AC_MSG_NOTICE(${LINKTESTS_DISABLED})
fi

AM_CONDITIONAL([RELACS_LINKTESTS], [test ${RELACS_LINKTESTS_ENABLED} = yes])



#################################################
## Doxygen, GraphViz, LaTeX
#################################################
DOXYGEN_ENABLED=no
## Doxygen
AC_CHECK_PROG(DOXY_CHECK, doxygen, found, missing)
if test ${DOXY_CHECK} != missing ; then
	## Graphviz
	AC_CHECK_PROG(GRAPHVIZ_CHECK, dot, found, missing)
	if test ${GRAPHVIZ_CHECK} != missing ; then
		## LaTeX
		AC_CHECK_PROG(LATEX_CHECK, latex, found, missing)
		if test ${LATEX_CHECK} != missing ; then
			DOXYGEN_ENABLED=yes
		fi
	fi
fi

AM_CONDITIONAL([RELACS_CREATE_DOC], [test ${DOXYGEN_ENABLED} = yes])

DOXYGEN_INPUT="\
config/src \
daq/src \
datafile/src \
mplot/src \
numerics/src \
options/src \
widgets/src \
\
relacs \
src \
\
hardware \
common \
auditory \
\
doc \
utils \
"
AC_SUBST(DOXYGEN_INPUT)



# Global flags
CPPFLAGS="${CPPFLAGS} -DRELACSDATA -DPLUGINVERSION=VERSION"

RELACS_CFG_HEAD=src/relacs_cfg_head
AC_SUBST_FILE(RELACS_CFG_HEAD)
RELACS_CFG_FOOT=src/relacs_cfg_foot
AC_SUBST_FILE(RELACS_CFG_FOOT)

RELACS_RUNNER_HEAD=src/relacs_runner_head
AC_SUBST_FILE(RELACS_RUNNER_HEAD)
RELACS_RUNNER_FOOT=src/relacs_runner_foot
AC_SUBST_FILE(RELACS_RUNNER_FOOT)



# Dump build config
awk -F"'" 'function pp(s) { return sprintf("%s = [[@%s@]]", s, s)}; start { print pp($1); \
    if (NF > 1) exit }; $1 == "ac_subst_vars=" {start=1; print pp($2)}' configure\
    | sort -f > build_config_dump.in



# Substitute
AC_CONFIG_FILES([
    build_config_dump
    
    auditory/base/Makefile
    auditory/sascha/Makefile
    auditory/speaker/Makefile
    auditory/Makefile
    common/base/Makefile
    common/controls/Makefile
    common/detectors/Makefile
    common/filter/Makefile
    common/models/Makefile
    common/Makefile
    config/src/Makefile
    config/Makefile
    daq/src/Makefile
    daq/Makefile
    datafile/src/Makefile
    datafile/Makefile
    hardware/Makefile
    mplot/src/Makefile
    mplot/Makefile
    numerics/src/Makefile
    numerics/Makefile
    options/src/Makefile
    options/Makefile
    relacs/Makefile
    relacs/src/Makefile
    src/Makefile
    utils/Makefile
    widgets/src/Makefile
    widgets/Makefile
    Makefile

    doc/relacs.doc
    doc/relacs.dox

    relacslocal.cfg
    src/relacs.cfg

    numerics/configure.ac
])

AC_CONFIG_FILES([relacslocal], [chmod a+x relacslocal])
AC_CONFIG_FILES([src/relacs], [chmod a+x src/relacs])

AC_OUTPUT



cat <<SUMMARY

    Use GSL ................ ${RELACS_GSL}
    Build link tests ....... ${RELACS_LINKTESTS_ENABLED}
    Create documentation ... ${DOXYGEN_ENABLED}

If you can read these lines ./configure finished sucessfully.
Relacs should build and install smoothly. If it doesn't please
open a bug describing your problem and configuration at
http://sourceforge.net/tracker/?group_id=199846&atid=971134

Continue with \$ make && sudo make install

SUMMARY

