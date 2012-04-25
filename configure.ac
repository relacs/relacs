# Process this file with autoconf to produce a configure script.

AC_INIT(relacs, 0.9.7)
AC_CONFIG_MACRO_DIR([m4 numerics/m4 \
    plugins/linuxdevices/comedi/m4 plugins/linuxdevices/daqflex/m4])

AM_INIT_AUTOMAKE([1.10 -Wall gnits subdir-objects dist-bzip2])

# no static libraries:
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PROG_LN_S
AC_PROG_CXX
AC_LANG(C++)

#################################################
## Check for presence of C++ compiler:
#################################################
RELACS_CHECK_CXX()

CXXFLAGS="-Wall -g ${CXXFLAGS}"
CFLAGS="-Wall $CFLAGS"

AM_PROG_CC_C_O


# Backup before any additions
CLEAN_CPPFLAGS=${CPPFLAGS}
CLEAN_LDFLAGS=${LDFLAGS}
CLEAN_LIBS=${LIBS}


#################################################
## GNU make
#################################################
AC_RELACS_GNU_MAKE()


#################################################
## pkg-config
#################################################
RELACS_CHECK_PKG_CONFIG()


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
RELACS_LIB_MATH()


#################################################
## GSL
#################################################
RELACS_LIB_GSL()


#################################################
## Qt4, moc
#################################################
RELACS_LIB_QT4()


#################################################
## RTAI and Comedi
#################################################
RELACS_HEADER_RTAI()
AM_CONDITIONAL(RELACS_COND_RTAI,[test "$RELACS_RTAI" != no])
RELACS_LIB_COMEDI()


#################################################
## DAQFlex for measurement computing USB boards
#################################################
RELACS_LIB_USB()
RELACS_DAQFLEX=${RELACS_USB}


#################################################
## TML_lib (for Mirob robot)
#################################################
RELACS_LIB_TML()
AM_CONDITIONAL(RELACS_COND_TML,[test "$RELACS_TML" != no])


#################################################
## RELACSDAQ
#################################################
DOXYGEN_PREDEF="$DOXYGEN_PREDEF HAVE_LIBRELACSDAQ"
AC_DEFINE([HAVE_LIBRELACSDAQ], [1],
 [Define if relacs daq library exists.])


#################################################
## Examples
#################################################
RELACS_EXAMPLES()


#################################################
## Docbook user manual
#################################################
RELACS_DOCBOOK()


#################################################
## API documentation directories
#################################################

# list of libraries that should go 
# into the main documentation:
RELACS_DOCLIBS="\
daq \
datafile \
plot \
numerics \
options \
widgets \
relacs \
"

# list of directories with their own documentation:
DOXYGEN_EXTERNAL="${RELACS_DOCLIBS}"


#################################################
## Plugin sets
#################################################

RELACS_PLUGINSET([comedi],[linuxdevices/comedi],[],[test "$RELACS_COMEDI" != no])
AC_CONFIG_FILES([plugins/linuxdevices/comedi/examples/Makefile])
AC_CONFIG_FILES([plugins/linuxdevices/comedi/module/Makefile])
RELACS_PLUGINSET([daqflex],[linuxdevices/daqflex],[],[test "$RELACS_USB" != no])
RELACS_NOPLUGINSET([nieseries],[linuxdevices/nieseries])
RELACS_PLUGINSET([attcs3310],[linuxdevices/attcs3310])
RELACS_PLUGINSET([misc],[linuxdevices/misc])
AC_CONFIG_FILES([plugins/linuxdevices/misc/examples/Makefile])
RELACS_PLUGINSET([base],[base])
RELACS_PLUGINSET([calibration],[calibration])
RELACS_PLUGINSET([ephys],[ephys])
RELACS_PLUGINSET([acoustic],[acoustic])
RELACS_PLUGINSET([auditory],[auditory],[ephys acoustic])
RELACS_PLUGINSET([auditoryprojects],[auditoryprojects],[ephys acoustic auditory])
RELACS_PLUGINSET([patchclamp],[patchclamp],[ephys])
RELACS_NOPLUGINSET([patchclampprojects],[patchclampprojects],[ephys])
RELACS_PLUGINSET([efield],[efield],[base])
RELACS_PLUGINSET([efish],[efish],[base ephys efield])
RELACS_NOPLUGINSET([multielectrode],[multielectrode],[ephys])

AC_SUBST([RELACS_PLUGINS_SUBDIRS])
AC_SUBST([RELACS_PLUGINS_DIST_SUBDIRS])


#################################################
## Doxygen API documentation
#################################################

# assemble input pathes from the Makefile.am of the RELACS_DOCLIBS list:
DOXYGEN_INPUT="doc src utils"
for FILE in $RELACS_DOCLIBS; do
	INPUTLINE=$(grep 'DX_SETTINGS += INPUT' $FILE/Makefile.am)
	INPUTLIST=${INPUTLINE#DX_SETTINGS += INPUT=\'}
	INPUTLIST=${INPUTLIST%\'}
	for INPUT in $INPUTLIST; do
	  DOXYGEN_INPUT="$DOXYGEN_INPUT $FILE/$INPUT"
	done
done

DOXYGEN_IMAGEPATHES="doc utils"
for dir in $RELACS_DOCLIBS; do
	DOXYGEN_IMAGEPATHES="$DOXYGEN_IMAGEPATHES ${dir}/doc"
done

AC_SUBST(RELACS_DOCLIBS)
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


#################################################
## RELACS runner scripts
#################################################

# Head of global relacs runner script:
RELACS_HEAD=$srcdir/src/relacs_head
AC_SUBST_FILE(RELACS_HEAD)

# Head of relacslocal runner scripts:
RELACSLOCAL_HEAD=$srcdir/src/relacslocal_head
AC_SUBST_FILE(RELACSLOCAL_HEAD)

# Foot of relacs runner script:
RELACS_FOOT=$srcdir/src/relacs_foot
AC_SUBST_FILE(RELACS_FOOT)

AC_CONFIG_FILES([src/relacs], [chmod a+x src/relacs])
AC_CONFIG_FILES([relacslocal], [chmod a+x relacslocal])
AC_CONFIG_FILES([plugins/base/relacslocal], [chmod a+x plugins/base/relacslocal])
AC_CONFIG_FILES([plugins/calibration/relacslocal], [chmod a+x plugins/calibration/relacslocal])
AC_CONFIG_FILES([plugins/acoustic/relacslocal], [chmod a+x plugins/acoustic/relacslocal])
AC_CONFIG_FILES([plugins/auditory/relacslocal], [chmod a+x plugins/auditory/relacslocal])
AC_CONFIG_FILES([plugins/auditoryprojects/relacslocal], [chmod a+x plugins/auditoryprojects/relacslocal])
AC_CONFIG_FILES([plugins/patchclamp/relacslocal], [chmod a+x plugins/patchclamp/relacslocal])
AC_CONFIG_FILES([plugins/patchclampprojects/relacslocal], [chmod a+x plugins/patchclampprojects/relacslocal])
AC_CONFIG_FILES([plugins/efield/relacslocal], [chmod a+x plugins/efield/relacslocal])
AC_CONFIG_FILES([plugins/efish/relacslocal], [chmod a+x plugins/efish/relacslocal])
AC_CONFIG_FILES([plugins/multielectrode/relacslocal], [chmod a+x plugins/multielectrode/relacslocal])


#################################################
## RELACS general
#################################################

# Relacs build date:
RELACSDATE=`date '+%m/%d/%y'`
AC_SUBST(RELACSDATE)

# Relacs version and build date as a preprocessor define:
#AC_DEFINE_UNQUOTED([RELACSVERSION],["${VERSION} (${RELACSDATE})"])
# Debian occasionally rebuilds identical source packages. As there is no change in
# its functionality it is better not to change the version. Moreover, this date
# makes it impossible to benefit from ccache for two consecutive days.
AC_DEFINE_UNQUOTED([RELACSVERSION],["${VERSION}"])

# All relacs libraries, the program, and the standard plugins
# are built from a common source tree.
AM_CONDITIONAL([RELACS_TOP_BUILD], [test 0 = 0])


#################################################
## File substitutions:
#################################################

AC_CONFIG_FILES([
    Makefile

    numerics/configure.ac
    numerics/Makefile
    numerics/src/Makefile
    numerics/examples/Makefile
    
    daq/configure.ac
    daq/Makefile
    daq/src/Makefile
    daq/examples/Makefile

    options/configure.ac
    options/Makefile
    options/src/Makefile
    options/examples/Makefile

    datafile/configure.ac
    datafile/Makefile
    datafile/src/Makefile
    datafile/examples/Makefile
    datafile/datatools/Makefile

    widgets/configure.ac
    widgets/Makefile
    widgets/src/Makefile
    widgets/examples/Makefile

    plot/configure.ac
    plot/Makefile
    plot/src/Makefile
    plot/examples/Makefile

    relacs/Makefile
    relacs/src/Makefile

    src/Makefile

    plugins/Makefile

    utils/Makefile

    examples/Makefile
    examples/src/Makefile

    doc/relacsall.doc

    doc/userman/Makefile
])


AC_OUTPUT


#################################################
## Summary:
#################################################

cat <<SUMMARY

    Use GSL ...................... ${RELACS_GSL}
    Use Comedi ................... ${RELACS_COMEDI}
    Use RTAI ..................... ${RELACS_RTAI}
    Use libusb ................... ${RELACS_USB}
    Use TML_lib .................. ${RELACS_TML}
    Generate user manual ......... ${RELACS_GENERATE_DOCBOOK}
    Generate API documentation ... ${DX_SUMMARY}
    Compile example programs ..... ${RELACS_COMPILE_EXAMPLES}
    Compile the following plugin sets:
SUMMARY

for RELACS_PLUGIN in ${RELACS_PLUGINS}; do
    echo "        ${RELACS_PLUGIN}"
done

cat <<SUMMARY

If you can read these lines ./configure finished sucessfully.
Relacs should build and install smoothly. If it doesn't please
open a bug describing your problem and configuration at
http://sourceforge.net/tracker/?group_id=199846&atid=971134

Continue with .... \$ make
Run a demo ....... \$ ./relacslocal -3
Install relacs ... \$ sudo make install

SUMMARY

