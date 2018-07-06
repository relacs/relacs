HEADER_PATH = ../include/relacs/photoreceptors
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libphotoreceptorssearch.la \
    libphotoreceptorspulses.la \
    libphotoreceptorstwochannelstimulus.la



libphotoreceptorssearch_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/plugins/base/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libphotoreceptorssearch_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libphotoreceptorssearch_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbasetraces.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libphotoreceptorssearch_la_OBJECTS) : moc_search.cc

libphotoreceptorssearch_la_SOURCES = search.cc

libphotoreceptorssearch_la_includedir = $(pkgincludedir)/photoreceptors

libphotoreceptorssearch_la_include_HEADERS = $(HEADER_PATH)/search.h



libphotoreceptorspulses_la_CPPFLAGS = \
    -I$(top_srcdir)/plugins/base/include \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libphotoreceptorspulses_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libphotoreceptorspulses_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbasetraces.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libphotoreceptorspulses_la_OBJECTS) : moc_pulses.cc

libphotoreceptorspulses_la_SOURCES = pulses.cc

libphotoreceptorspulses_la_includedir = $(pkgincludedir)/photoreceptors

libphotoreceptorspulses_la_include_HEADERS = $(HEADER_PATH)/pulses.h



libphotoreceptorstwochannelstimulus_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libphotoreceptorstwochannelstimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libphotoreceptorstwochannelstimulus_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libphotoreceptorstwochannelstimulus_la_OBJECTS) : moc_twochannelstimulus.cc

libphotoreceptorstwochannelstimulus_la_SOURCES = twochannelstimulus.cc

libphotoreceptorstwochannelstimulus_la_includedir = $(pkgincludedir)/photoreceptors

libphotoreceptorstwochannelstimulus_la_include_HEADERS = $(HEADER_PATH)/twochannelstimulus.h



check_PROGRAMS = \
    linktest_libphotoreceptorssearch_la \
    linktest_libphotoreceptorspulses_la \
    linktest_libphotoreceptorstwochannelstimulus_la

linktest_libphotoreceptorssearch_la_SOURCES = linktest.cc
linktest_libphotoreceptorssearch_la_LDADD = libphotoreceptorssearch.la

linktest_libphotoreceptorspulses_la_SOURCES = linktest.cc
linktest_libphotoreceptorspulses_la_LDADD = libphotoreceptorspulses.la

linktest_libphotoreceptorstwochannelstimulus_la_SOURCES = linktest.cc
linktest_libphotoreceptorstwochannelstimulus_la_LDADD = libphotoreceptorstwochannelstimulus.la

TESTS = $(check_PROGRAMS)
