# Process this file with ./bootstrap.sh to produce a configure script.

AC_INIT(relacs, 0.9.8)

m4_ifdef( [AC_CONFIG_MACRO_DIRS],
	  [ AC_CONFIG_MACRO_DIRS([m4 numerics/m4 \
	    plugins/linuxdevices/comedi/m4 \
	    plugins/linuxdevices/rtaicomedi/m4 \
	    plugins/linuxdevices/daqflex/m4]) ],
	  [ AC_CONFIG_MACRO_DIR([m4 numerics/m4 \
	    plugins/linuxdevices/comedi/m4 \
	    plugins/linuxdevices/rtaicomedi/m4 \
	    plugins/linuxdevices/daqflex/m4]) ] )

AM_INIT_AUTOMAKE([1.10 -Wall gnits subdir-objects dist-bzip2])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# no static libraries:
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PROG_LN_S
AC_PROG_CXX
AC_LANG(C++)

# default installation directory:
AC_PREFIX_DEFAULT([/usr])

#################################################
## Check for presence of C++ compiler:
#################################################
RELACS_CHECK_CXX()

CXXFLAGS="-Wall -g -std=c++11 ${CXXFLAGS}"
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
## AUDIO
#################################################
RELACS_LIB_SNDFILE()
RELACS_LIB_PORTAUDIO()


#################################################
## Qt4, moc
#################################################
RELACS_LIB_QT4()


#################################################
## Comedi and RTAI
#################################################
RELACS_LIB_COMEDI()
AM_CONDITIONAL(RELACS_COND_COMEDIROUTING,[test "$COMEDI_SET_ROUTING" = yes])

RELACS_HEADER_RTAI()
AM_CONDITIONAL(RELACS_COND_RTAI,[test "$RELACS_COMEDI" != no -a  "$RELACS_RTAI" != no])

RELACS_DYNCLAMP()


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
## OpenCV (for camera)
#################################################
RELACS_LIB_OPENCV()
AM_CONDITIONAL(RELACS_COND_OPENCV,[test "$RELACS_OPENCV" != no])


#################################################
## NIX IO Library
#################################################
RELACS_LIB_NIX()
AM_CONDITIONAL(RELACS_COND_NIX, [test "$RELACS_NIX" != no])


#################################################
## RELACSSHAPES (for Str, Options)
#################################################
DOXYGEN_PREDEF="$DOXYGEN_PREDEF HAVE_LIBRELACSSHAPES"
AC_DEFINE([HAVE_LIBRELACSSHAPES], [1],
 [Define if relacs shapes library exists.])


#################################################
## RELACSDAQ (for Plot)
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
shapes \
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

RELACS_PLUGINSET([comedi],[linuxdevices/comedi],[],[test x$RELACS_COMEDI != xno])
AC_CONFIG_FILES([plugins/linuxdevices/comedi/examples/Makefile])
RELACS_PLUGINSET([rtaicomedi],[linuxdevices/rtaicomedi],[comedi],[test x$RELACS_RTAI != xno])
AC_CONFIG_FILES([plugins/linuxdevices/rtaicomedi/module/Makefile])
RELACS_PLUGINSET([daqflex],[linuxdevices/daqflex],[],[test x$RELACS_USB != xno])
RELACS_NOPLUGINSET([nieseries],[linuxdevices/nieseries])
RELACS_PLUGINSET([attcs3310],[linuxdevices/attcs3310])
RELACS_PLUGINSET([misc],[linuxdevices/misc])
AC_CONFIG_FILES([plugins/linuxdevices/misc/examples/Makefile])
RELACS_PLUGINSET([base],[base])
RELACS_PLUGINSET([calibration],[calibration])
RELACS_NOPLUGINSET([camera],[camera],[],[test x$RELACS_OPENCV != xno])
RELACS_PLUGINSET([ephys],[ephys])
RELACS_PLUGINSET([acoustic],[acoustic])
RELACS_PLUGINSET([auditory],[auditory],[ephys acoustic])
RELACS_NOPLUGINSET([auditoryprojects],[auditoryprojects],[ephys acoustic auditory])
RELACS_PLUGINSET([patchclamp],[patchclamp],[ephys])
RELACS_NOPLUGINSET([patchclampprojects],[patchclampprojects],[ephys])
RELACS_PLUGINSET([efield],[efield],[base])
RELACS_PLUGINSET([efish],[efish],[base ephys efield])
RELACS_NOPLUGINSET([multielectrode],[multielectrode],[ephys])
RELACS_PLUGINSET([photoreceptors],[photoreceptors],[])
RELACS_NOPLUGINSET([voltageclamp],[voltageclamp],[])

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
AC_CONFIG_FILES([plugins/camera/relacslocal], [chmod a+x plugins/camera/relacslocal])
AC_CONFIG_FILES([plugins/ephys/relacslocal], [chmod a+x plugins/ephys/relacslocal])
AC_CONFIG_FILES([plugins/acoustic/relacslocal], [chmod a+x plugins/acoustic/relacslocal])
AC_CONFIG_FILES([plugins/auditory/relacslocal], [chmod a+x plugins/auditory/relacslocal])
AC_CONFIG_FILES([plugins/auditoryprojects/relacslocal], [chmod a+x plugins/auditoryprojects/relacslocal])
AC_CONFIG_FILES([plugins/patchclamp/relacslocal], [chmod a+x plugins/patchclamp/relacslocal])
AC_CONFIG_FILES([plugins/patchclampprojects/relacslocal], [chmod a+x plugins/patchclampprojects/relacslocal])
AC_CONFIG_FILES([plugins/efield/relacslocal], [chmod a+x plugins/efield/relacslocal])
AC_CONFIG_FILES([plugins/efish/relacslocal], [chmod a+x plugins/efish/relacslocal])
AC_CONFIG_FILES([plugins/multielectrode/relacslocal], [chmod a+x plugins/multielectrode/relacslocal])
AC_CONFIG_FILES([plugins/linuxdevices/comedi/relacslocal], [chmod a+x plugins/linuxdevices/comedi/relacslocal])
AC_CONFIG_FILES([plugins/linuxdevices/rtaicomedi/relacslocal], [chmod a+x plugins/linuxdevices/rtaicomedi/relacslocal])
AC_CONFIG_FILES([plugins/linuxdevices/daqflex/relacslocal], [chmod a+x plugins/linuxdevices/daqflex/relacslocal])
AC_CONFIG_FILES([plugins/linuxdevices/attcs3310/relacslocal], [chmod a+x plugins/linuxdevices/attcs3310/relacslocal])
AC_CONFIG_FILES([plugins/linuxdevices/misc/relacslocal], [chmod a+x plugins/linuxdevices/misc/relacslocal])
AC_CONFIG_FILES([plugins/photoreceptors/relacslocal], [chmod a+x plugins/photoreceptors/relacslocal])
AC_CONFIG_FILES([plugins/voltageclamp/relacslocal], [chmod a+x plugins/voltageclamp/relacslocal])

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
    
    shapes/configure.ac
    shapes/Makefile
    shapes/src/Makefile
    
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
    C++ language standard ........ ${CXXSTD}
    Use GSL ...................... ${RELACS_GSL}
    Use sndfile .................. ${RELACS_SNDFILE}
    Use portaudio ................ ${RELACS_PORTAUDIO}
    Use comedi ................... ${RELACS_COMEDI}
    Use RTAI ..................... ${RELACS_RTAI} ${RELACS_RTAI_VERSION}
    Use libusb ................... ${RELACS_USB}
    Use TML_lib .................. ${RELACS_TML}
    Use OpenCV ................... ${RELACS_OPENCV}
    Use NIX IO ................... ${RELACS_NIX}

    Dynamic clamp configuration ..${RELACS_DYNCLAMP_SUMMARY}
    Generate user manual ......... ${RELACS_GENERATE_DOCBOOK}
    Generate API documentation ... ${DX_SUMMARY}
    Compile example programs ..... ${RELACS_COMPILE_EXAMPLES}

SUMMARY

if test "x${RELACS_PLUGINS_ENABLE}" != "x"; then
   echo "    Document the following plugin sets:"
   for RELACS_PLUGIN in ${RELACS_PLUGINS_ENABLE}; do
       echo "        ${RELACS_PLUGIN}"
   done
fi

echo "    Compile and document the following plugin sets:"
for RELACS_PLUGIN in ${RELACS_PLUGINS_COMPILE}; do
    echo "        ${RELACS_PLUGIN}"
done

cat <<SUMMARY

If you can read these lines ./configure finished sucessfully.
Relacs should build and install smoothly.

Continue with .... \$ make
Run a demo ....... \$ ./relacslocal -3
Install relacs ... \$ sudo make install

SUMMARY



