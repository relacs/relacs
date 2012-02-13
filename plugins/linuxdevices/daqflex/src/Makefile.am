HEADER_PATH = ../include/relacs/daqflex

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libdaqflex.la



libdaqflex_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(USB_CPPFLAGS) \
    $(QT4_CPPFLAGS)

libdaqflex_la_LDFLAGS = \
    -module -avoid-version \
    $(USB_LDFLAGS) \
    $(QT4_LDFLAGS)

libdaqflex_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(USB_LIBS) \
    $(QT4_LIBS)

libdaqflex_la_SOURCES = \
    daqflexcore.cc \
    daqflexanaloginput.cc \
    daqflexanalogoutput.cc \
    daqflexdevices.cc


libdaqflex_la_includedir = $(pkgincludedir)/daqflex

libdaqflex_la_include_HEADERS = \
    $(HEADER_PATH)/daqflexcore.h \
    $(HEADER_PATH)/daqflexanaloginput.h \
    $(HEADER_PATH)/daqflexanalogoutput.h


check_PROGRAMS = \
    linktest_libdaqflex_la

linktest_libdaqflex_la_SOURCES = linktest.cc
linktest_libdaqflex_la_LDADD = libdaqflex.la

TESTS = $(check_PROGRAMS)
