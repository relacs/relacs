lib_LTLIBRARIES = \
    librelacsdaq.la


if RELACS_TOP_BUILD
librelacsdaq_la_CPPFLAGS = \
    @GSL_CPPFLAGS@ \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../include
else
librelacsdaq_la_CPPFLAGS = \
    @GSL_CPPFLAGS@ \
    -I$(srcdir)/../include
endif

librelacsdaq_la_LDFLAGS = \
    @GSL_LDFLAGS@

if RELACS_TOP_BUILD
librelacsdaq_la_LIBADD = \
    ../../numerics/src/librelacsnumerics.la \
    @GSL_LIBS@
else
librelacsdaq_la_LIBADD = \
    -lrelacsnumerics \
    @GSL_LIBS@
endif

librelacsdaq_ladir = $(includedir)/relacs

librelacsdaq_la_HEADERS = \
    ../include/relacs/acquire.h \
    ../include/relacs/analoginput.h \
    ../include/relacs/analogoutput.h \
    ../include/relacs/attenuate.h \
    ../include/relacs/attenuator.h \
    ../include/relacs/daqerror.h \
    ../include/relacs/device.h \
    ../include/relacs/indata.h \
    ../include/relacs/inlist.h \
    ../include/relacs/manipulator.h \
    ../include/relacs/outdata.h \
    ../include/relacs/outlist.h \
    ../include/relacs/temperature.h \
    ../include/relacs/tracespec.h

librelacsdaq_la_SOURCES = \
    acquire.cc \
    analoginput.cc \
    analogoutput.cc \
    attenuate.cc \
    attenuator.cc \
    daqerror.cc \
    device.cc \
    indata.cc \
    inlist.cc \
    manipulator.cc \
    outdata.cc \
    outlist.cc \
    temperature.cc \
    tracespec.cc


check_PROGRAMS = linktest_librelacsdaq_la
linktest_librelacsdaq_la_SOURCES = linktest.cc
linktest_librelacsdaq_la_LDADD = librelacsdaq.la

