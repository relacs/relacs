HEADER_PATH = ../include/relacs/comedi

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcomedicomedianaloginput.la \
    libcomedicomedianalogoutput.la \
    libcomedidynclampanaloginput.la \
    libcomedidynclampanalogoutput.la



libcomedicomedianaloginput_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcomedicomedianaloginput_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedicomedianaloginput_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    libcomedicomedianalogoutput.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

libcomedicomedianaloginput_la_SOURCES = \
    comedianaloginput.cc \
    comedianaloginputdevice.cc

libcomedicomedianaloginput_la_includedir = $(pkgincludedir)/comedi

libcomedicomedianaloginput_la_include_HEADERS = \
    $(HEADER_PATH)/comedianaloginput.h



libcomedicomedianalogoutput_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcomedicomedianalogoutput_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedicomedianalogoutput_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

libcomedicomedianalogoutput_la_SOURCES = \
    comedianalogoutput.cc \
    comedianalogoutputdevice.cc

libcomedicomedianalogoutput_la_includedir = $(pkgincludedir)/comedi

libcomedicomedianalogoutput_la_include_HEADERS = \
    $(HEADER_PATH)/comedianalogoutput.h



libcomedidynclampanaloginput_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    -I/usr/src/rtai-3.6/base/include \
    $(QT3_CPPFLAGS)

libcomedidynclampanaloginput_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedidynclampanaloginput_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    libcomedicomedianaloginput.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

libcomedidynclampanaloginput_la_SOURCES = \
    dynclampanaloginput.cc \
    dynclampanaloginputdevice.cc

libcomedidynclampanaloginput_la_includedir = $(pkgincludedir)/comedi

libcomedidynclampanaloginput_la_include_HEADERS = \
    $(HEADER_PATH)/moduledef.h \
    $(HEADER_PATH)/dynclampanaloginput.h



libcomedidynclampanalogoutput_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    -I/usr/src/rtai-3.6/base/include \
    $(QT3_CPPFLAGS)

libcomedidynclampanalogoutput_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedidynclampanalogoutput_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    libcomedicomedianalogoutput.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

libcomedidynclampanalogoutput_la_SOURCES = \
    dynclampanalogoutput.cc \
    dynclampanalogoutputdevice.cc

libcomedidynclampanalogoutput_la_includedir = $(pkgincludedir)/comedi

libcomedidynclampanalogoutput_la_include_HEADERS = \
    $(HEADER_PATH)/moduledef.h \
    $(HEADER_PATH)/dynclampanalogoutput.h



check_PROGRAMS = \
    linktest_libcomedicomedianaloginput_la \
    linktest_libcomedicomedianalogoutput_la \
    linktest_libcomedidynclampanaloginput_la \
    linktest_libcomedidynclampanalogoutput_la

linktest_libcomedicomedianaloginput_la_SOURCES = linktest.cc
linktest_libcomedicomedianaloginput_la_LDADD = libcomedicomedianaloginput.la

linktest_libcomedicomedianalogoutput_la_SOURCES = linktest.cc
linktest_libcomedicomedianalogoutput_la_LDADD = libcomedicomedianalogoutput.la

linktest_libcomedidynclampanaloginput_la_SOURCES = linktest.cc
linktest_libcomedidynclampanaloginput_la_LDADD = libcomedidynclampanaloginput.la

linktest_libcomedidynclampanalogoutput_la_SOURCES = linktest.cc
linktest_libcomedidynclampanalogoutput_la_LDADD = libcomedidynclampanalogoutput.la

TESTS = $(check_PROGRAMS)
