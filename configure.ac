# Process this file with autoconf to produce a configure script.

AC_INIT(relacs, 0.9.6)
AM_INIT_AUTOMAKE([1.10 foreign subdir-objects dist-zip dist-bzip2])

AC_PROG_LN_S
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
## Link tests
#################################################
AC_RELACS_LINK_TESTS()


#################################################
## Doxygen
#################################################

DOXYGEN_INPUT="\
config/src \
config/include/relacs \
daq/src \
daq/include/relacs \
datafile/src \
datafile/include/relacs \
numerics/src \
numerics/include/relacs \
mplot/src \
mplot/include/relacs \
options/src \
options/include/relacs \
widgets/src \
widgets/include/relacs \
\
relacs/src \
relacs/include/relacs \
src \
\
doc \
utils \
"

DOXYGEN_EXTERNAL="\
config \
daq \
datafile \
mplot \
numerics \
options \
widgets \
relacs \
"

DOXYGEN_TAGFILES=""
for dir in $DOXYGEN_EXTERNAL; do
	DOXYGEN_TAGFILES="$DOXYGEN_TAGFILES ${dir}/doc/${dir}.tag=${dir}"
done

DOXYGEN_IMAGEPATHES="doc"
for dir in $DOXYGEN_EXTERNAL; do
	DOXYGEN_IMAGEPATHES="$DOXYGEN_IMAGEPATHES ${dir}/doc"
done

AC_SUBST(DOXYGEN_INPUT)
AC_SUBST(DOXYGEN_EXTERNAL)
AC_SUBST(DOXYGEN_TAGFILES)
AC_SUBST(DOXYGEN_IMAGEPATHES)

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN( $PACKAGE_NAME, doc/relacsall.dox, doc )


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

    config/src/Makefile
    config/Makefile
    config/doc/config.dox
    config/configure.ac

    daq/src/Makefile
    daq/Makefile
    daq/doc/daq.dox
    daq/configure.ac

    datafile/src/Makefile
    datafile/Makefile
    datafile/doc/datafile.dox
    datafile/configure.ac

    hardware/src/Makefile
    hardware/Makefile

    mplot/src/Makefile
    mplot/Makefile
    mplot/doc/mplot.dox
    mplot/configure.ac

    numerics/src/Makefile
    numerics/Makefile
    numerics/doc/numerics.dox
    numerics/configure.ac

    options/src/Makefile
    options/Makefile
    options/doc/options.dox
    options/configure.ac

    relacs/Makefile
    relacs/src/Makefile
    relacs/doc/relacs.dox

    src/Makefile

    utils/Makefile

    widgets/src/Makefile
    widgets/Makefile
    widgets/doc/widgets.dox
    widgets/configure.ac

    Makefile

    doc/relacsall.doc
    doc/relacsall.dox

    relacslocal.cfg
    src/relacs.cfg
])

AC_CONFIG_FILES([relacslocal], [chmod a+x relacslocal])
AC_CONFIG_FILES([src/relacs], [chmod a+x src/relacs])

AC_OUTPUT



cat <<SUMMARY

    Use GSL ................ ${RELACS_GSL}
    Use Comedi ............. ${RELACS_COMEDI}
    Build link tests ....... ${RELACS_LINKTESTS_ENABLED}
    Create documentation ... ${DX_SUMMARY}

If you can read these lines ./configure finished sucessfully.
Relacs should build and install smoothly. If it doesn't please
open a bug describing your problem and configuration at
http://sourceforge.net/tracker/?group_id=199846&atid=971134

Continue with \$ make && sudo make install

SUMMARY

