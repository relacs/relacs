# Process this file with autoconf to produce a configure script.

AC_INIT(relacs, 0.9.6)
AM_INIT_AUTOMAKE([1.10 -Wall foreign subdir-objects dist-zip dist-bzip2])

AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PROG_LN_S
AC_PROG_CXX
AC_LANG(C++)

CXXFLAGS="-Wall $CXXFLAGS"
CFLAGS="-Wall $CFLAGS"

AM_PROG_CC_C_O

# Static substitutions
RELACSDATE=`date '+%m/%d/%y'`
AC_SUBST(RELACSDATE)

RELACSVERSION="${VERSION} (${RELACSDATE})"
AC_SUBST(RELACSVERSION)


# Backup before any additions
CLEAN_CPPFLAGS=${CPPFLAGS}
CLEAN_LDFLAGS=${LDFLAGS}
CLEAN_LIBS=${LIBS}


#################################################
## GNU make
#################################################
AC_RELACS_GNU_MAKE()


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


# Restore
CPPFLAGS=${CLEAN_CPPFLAGS}
LDFLAGS=${CLEAN_LDFLAGS}
LIBS=${CLEAN_LIBS}


#################################################
## math
#################################################
AC_RELACS_CHECK_MATH()


#################################################
## GSL
#################################################
AC_RELACS_CHECK_GSL()


#################################################
## Comedi
#################################################
AC_RELACS_CHECK_COMEDI()


#################################################
## Qt3, moc
#################################################
AC_RELACS_CHECK_QT3()


#################################################
## RELACSDAQ
#################################################
DOXYGEN_PREDEF="$DOXYGEN_PREDEF HAVE_LIBRELACSDAQ"
AC_DEFINE([HAVE_LIBRELACSDAQ], [1],
 [Define if relacs daq library exists.])


#################################################
## Doxygen
#################################################

DOXYGEN_EXTERNAL="\
daq \
datafile \
plot \
numerics \
options \
widgets \
relacs \
hardware \
common \
auditory \
"

# assemble input pathes from the Makefile.am of the DOXYGEN_EXTERNAL list:
DOXYGEN_INPUT="doc src utils"
for FILE in $DOXYGEN_EXTERNAL src; do
	INPUTLINE=$(grep 'DX_SETTINGS += INPUT' $FILE/Makefile.am)
	INPUTLIST=${INPUTLINE#DX_SETTINGS += INPUT=\'}
	INPUTLIST=${INPUTLIST%\'}
	for INPUT in $INPUTLIST; do
	  DOXYGEN_INPUT="$DOXYGEN_INPUT $FILE/$INPUT"
	done
done

DOXYGEN_IMAGEPATHES="doc"
for dir in $DOXYGEN_EXTERNAL; do
	DOXYGEN_IMAGEPATHES="$DOXYGEN_IMAGEPATHES ${dir}/doc"
done

AC_SUBST(DOXYGEN_EXTERNAL)
AC_SUBST(DOXYGEN_INPUT)
AC_SUBST(DOXYGEN_IMAGEPATHES)

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN( relacsall, doc/Doxyfile, doc )


# All relacs libraries, the program, and the standard plugins
# are built from a common source tree.
AM_CONDITIONAL([RELACS_TOP_BUILD], [test 0 = 0])


# Global flags
CPPFLAGS="${CPPFLAGS} -DPLUGINVERSION=\\\"$VERSION\\\""

RELACS_CFG_HEAD=src/relacs_cfg_head
AC_SUBST_FILE(RELACS_CFG_HEAD)
RELACS_CFG_FOOT=src/relacs_cfg_foot
AC_SUBST_FILE(RELACS_CFG_FOOT)

RELACS_RUNNER_HEAD=src/relacs_runner_head
AC_SUBST_FILE(RELACS_RUNNER_HEAD)
RELACS_RUNNER_FOOT=src/relacs_runner_foot
AC_SUBST_FILE(RELACS_RUNNER_FOOT)



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

    daq/src/Makefile
    daq/Makefile
    daq/configure.ac

    datafile/src/Makefile
    datafile/Makefile
    datafile/configure.ac

    hardware/src/Makefile
    hardware/Makefile

    plot/src/Makefile
    plot/Makefile
    plot/configure.ac

    numerics/src/Makefile
    numerics/Makefile
    numerics/configure.ac

    options/src/Makefile
    options/Makefile
    options/configure.ac

    relacs/Makefile
    relacs/src/Makefile

    src/Makefile

    utils/Makefile

    widgets/src/Makefile
    widgets/Makefile
    widgets/configure.ac

    Makefile

    doc/relacsall.doc

    relacslocal.cfg
    src/relacs.cfg
])

AC_CONFIG_FILES([relacslocal], [chmod a+x relacslocal])
AC_CONFIG_FILES([src/relacs], [chmod a+x src/relacs])

AC_OUTPUT



cat <<SUMMARY

    Use GSL ................ ${RELACS_GSL}
    Use Comedi ............. ${RELACS_COMEDI}
    Create documentation ... ${DX_SUMMARY}

If you can read these lines ./configure finished sucessfully.
Relacs should build and install smoothly. If it doesn't please
open a bug describing your problem and configuration at
http://sourceforge.net/tracker/?group_id=199846&atid=971134

Continue with \$ make && sudo make install

SUMMARY

