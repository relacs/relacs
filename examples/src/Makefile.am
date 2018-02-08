HEADER_PATH = ../include/relacs/examples
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libexamplesreproexample.la



libexamplesreproexample_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libexamplesreproexample_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libexamplesreproexample_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libexamplesreproexample_la_OBJECTS) : moc_reproexample.cc

libexamplesreproexample_la_SOURCES = reproexample.cc

libexamplesreproexample_la_includedir = $(pkgincludedir)/examples

libexamplesreproexample_la_include_HEADERS = $(HEADER_PATH)/reproexample.h



check_PROGRAMS = \
    linktest_libexamplesreproexample_la

linktest_libexamplesreproexample_la_SOURCES = linktest.cc
linktest_libexamplesreproexample_la_LDADD = libexamplesreproexample.la

TESTS = $(check_PROGRAMS)


