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
AC_RELACS_GNU_MAKE()



#################################################
## Qt3, moc
#################################################
AC_RELACS_CHECK_QT3()



#################################################
## Comedi
#################################################
AC_RELACS_CHECK_COMEDI()



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
AC_RELACS_LINK_TESTS()



#################################################
## Doxygen, GraphViz, LaTeX
#################################################
AC_RELACS_CHECK_DOXYGEN()

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

