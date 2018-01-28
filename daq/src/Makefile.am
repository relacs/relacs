lib_LTLIBRARIES = \
    librelacsdaq.la


if RELACS_TOP_BUILD
librelacsdaq_la_CPPFLAGS = \
    $(QT4CORE_CPPFLAGS) \
    $(GSL_CPPFLAGS) \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include
else
librelacsdaq_la_CPPFLAGS = \
    $(QT4CORE_CPPFLAGS) \
    $(GSL_CPPFLAGS) \
    -I$(srcdir)/../include
endif

librelacsdaq_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(QT4CORE_LDFLAGS) \
    $(GSL_LDFLAGS)

if RELACS_TOP_BUILD
librelacsdaq_la_LIBADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    $(QT4CORE_LIBS) \
    $(GSL_LIBS)
else
librelacsdaq_la_LIBADD = \
    -lrelacsnumerics \
    -lrelacsoptions \
    $(QT4CORE_LIBS) \
    $(GSL_LIBS)
endif

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/acquire.h \
    ../include/relacs/analoginput.h \
    ../include/relacs/analogoutput.h \
    ../include/relacs/attenuate.h \
    ../include/relacs/attenuator.h \
    ../include/relacs/camera.h \
    ../include/relacs/daqerror.h \
    ../include/relacs/device.h \
    ../include/relacs/digitalio.h \
    ../include/relacs/indata.h \
    ../include/relacs/inlist.h \
    ../include/relacs/manipulator.h \
    ../include/relacs/matrix.h \
    ../include/relacs/outdatainfo.h \
    ../include/relacs/outdata.h \
    ../include/relacs/outlist.h \
    ../include/relacs/point.h \
    ../include/relacs/shape.h \
    ../include/relacs/temperature.h \
    ../include/relacs/tracespec.h \
    ../include/relacs/trigger.h \
    ../include/relacs/zones.h

librelacsdaq_la_SOURCES = \
    acquire.cc \
    analoginput.cc \
    analogoutput.cc \
    attenuate.cc \
    attenuator.cc \
    camera.cc \
    daqerror.cc \
    device.cc \
    digitalio.cc \
    indata.cc \
    inlist.cc \
    manipulator.cc \
    matrix.cc \
    outdatainfo.cc \
    outdata.cc \
    outlist.cc \
    point.cc \
    shape.cc \
    temperature.cc \
    tracespec.cc \
    trigger.cc \
    zones.cc


check_PROGRAMS = linktest_librelacsdaq_la \
    checkmatrix

linktest_librelacsdaq_la_SOURCES = linktest.cc
linktest_librelacsdaq_la_LDADD = librelacsdaq.la

checkmatrix_SOURCES = checkmatrix.cc
checkmatrix_CPPFLAGS = -I$(srcdir)/../include
checkmatrix_LDADD = librelacsdaq.la

TESTS = $(check_PROGRAMS)

