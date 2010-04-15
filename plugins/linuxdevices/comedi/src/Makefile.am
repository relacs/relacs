HEADER_PATH = ../include/relacs/comedi

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcomedistreaming.la
if RELACS_COND_RTAI
pluginlib_LTLIBRARIES += \
    libcomedidynclamp.la
endif


libcomedistreaming_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcomedistreaming_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedistreaming_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

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



libcomedidynclamp_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    -I/usr/src/linux/include \
    $(RTAI_CPPFLAGS) \
    $(QT3_CPPFLAGS)

libcomedidynclamp_la_LDFLAGS = \
    -module -avoid-version \
    $(COMEDI_LDFLAGS) \
    $(QT3_LDFLAGS)

libcomedidynclamp_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    libcomedistreaming.la \
    $(COMEDI_LIBS) \
    $(QT3_LIBS)

libcomedidynclamp_la_SOURCES = \
    dynclampanaloginput.cc \
    dynclampanalogoutput.cc \
    dynclamptrigger.cc \
    dynclamp.cc

libcomedidynclamp_la_includedir = $(pkgincludedir)/comedi

libcomedidynclamp_la_include_HEADERS = \
    $(HEADER_PATH)/moduledef.h \
    $(HEADER_PATH)/dynclampanaloginput.h \
    $(HEADER_PATH)/dynclampanalogoutput.h \
    $(HEADER_PATH)/dynclamptrigger.h



check_PROGRAMS = \
    linktest_libcomedistreaming_la
if RELACS_COND_RTAI
check_PROGRAMS += \
    linktest_libcomedidynclamp_la
endif

linktest_libcomedistreaming_la_SOURCES = linktest.cc
linktest_libcomedistreaming_la_LDADD = libcomedistreaming.la

linktest_libcomedidynclamp_la_SOURCES = linktest.cc
linktest_libcomedidynclamp_la_LDADD = libcomedidynclamp.la

TESTS = $(check_PROGRAMS)
