HEADER_PATH = ../include/relacs/patchclamp
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libpatchclampsimple.la



libpatchclampsimple_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libpatchclampsimple_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libpatchclampsimple_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libpatchclampsimple_la_OBJECTS) : moc_simple.cc

libpatchclampsimple_la_SOURCES = simple.cc

libpatchclampsimple_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampsimple_la_include_HEADERS = $(HEADER_PATH)/simple.h



check_PROGRAMS = \
    linktest_libpatchclampsimple_la

linktest_libpatchclampsimple_la_SOURCES = linktest.cc
linktest_libpatchclampsimple_la_LDADD = libpatchclampsimple.la

TESTS = $(check_PROGRAMS)

