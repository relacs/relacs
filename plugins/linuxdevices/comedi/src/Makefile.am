HEADER_PATH = ../include/relacs/comedi

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcomedistreaming.la


libcomedistreaming_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(COMEDI_CPPFLAGS) \
    $(QT4_CPPFLAGS)

libcomedistreaming_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT4_LDFLAGS)

libcomedistreaming_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(COMEDI_LIBS) \
    $(QT4_LIBS) $(GSL_LIBS)

libcomedistreaming_la_SOURCES = \
    comedianaloginput.cc \
    comedianalogoutput.cc \
    comedidigitalio.cc \
    comedirouting.cc \
    comedinipfi.cc \
    comedistreaming.cc

libcomedistreaming_la_includedir = $(pkgincludedir)/comedi

libcomedistreaming_la_include_HEADERS = \
    $(HEADER_PATH)/comedianaloginput.h \
    $(HEADER_PATH)/comedianalogoutput.h \
    $(HEADER_PATH)/comedidigitalio.h \
    $(HEADER_PATH)/comedirouting.h \
    $(HEADER_PATH)/comedinipfi.h



check_PROGRAMS = \
    linktest_libcomedistreaming_la

linktest_libcomedistreaming_la_SOURCES = linktest.cc
linktest_libcomedistreaming_la_LDADD = libcomedistreaming.la

TESTS = $(check_PROGRAMS)
