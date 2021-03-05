HEADER_PATH = ../include/relacs/rtaicomedi

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = libdynclampsim.la
if RELACS_COND_COMPILE_rtaicomedi
    pluginlib_LTLIBRARIES += librtaicomedidynclamp.la
endif


libdynclampsim_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    -I$(srcdir)/.. \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libdynclampsim_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libdynclampsim_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(COMEDI_LIBS) \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libdynclampsim_la_SOURCES = \
    dynclampmodelsim.cc \
    dynclampaisim.cc \
    dynclampaosim.cc

libdynclampsim_la_includedir = $(pkgincludedir)/rtaicomedi

libdynclampsim_la_include_HEADERS = \
    $(HEADER_PATH)/moduledef.h \
    $(HEADER_PATH)/dynclampmodelsim.h \
    $(HEADER_PATH)/dynclampaisim.h \
    $(HEADER_PATH)/dynclampaosim.h



librtaicomedidynclamp_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/linuxdevices/comedi/include \
    -I$(srcdir)/../include \
    -I$(srcdir)/.. \
    $(RTAI_CPPFLAGS) \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

librtaicomedidynclamp_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

librtaicomedidynclamp_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/plugins/linuxdevices/comedi/src/libcomedistreaming.la \
    $(COMEDI_LIBS) \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

librtaicomedidynclamp_la_SOURCES = \
    dynclampanaloginput.cc \
    dynclampanalogoutput.cc \
    dynclampdigitalio.cc \
    dynclamptrigger.cc \
    dynclamp.cc

librtaicomedidynclamp_la_includedir = $(pkgincludedir)/rtaicomedi

librtaicomedidynclamp_la_include_HEADERS = \
    $(HEADER_PATH)/moduledef.h \
    $(HEADER_PATH)/dynclampanaloginput.h \
    $(HEADER_PATH)/dynclampanalogoutput.h \
    $(HEADER_PATH)/dynclampdigitalio.h \
    $(HEADER_PATH)/dynclamptrigger.h



check_PROGRAMS = linktest_libdynclampsim_la
if RELACS_COND_COMPILE_rtaicomedi
    check_PROGRAMS += linktest_librtaicomedidynclamp_la
endif

linktest_libdynclampsim_la_SOURCES = linktest.cc
linktest_libdynclampsim_la_LDADD = libdynclampsim.la

if RELACS_COND_COMPILE_rtaicomedi
    linktest_librtaicomedidynclamp_la_SOURCES = linktest.cc
    linktest_librtaicomedidynclamp_la_LDADD = librtaicomedidynclamp.la
endif

TESTS = $(check_PROGRAMS)
