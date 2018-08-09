HEADER_PATH = ../include/relacs/voltageclamp
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libvoltageclampactivation.la



libvoltageclampactivation_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)$(GSL_CPPFLAGS)

libvoltageclampactivation_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclampactivation_la_LIBADD = \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libvoltageclampactivation_la_OBJECTS) : moc_activation.cc

libvoltageclampactivation_la_SOURCES = activation.cc

libvoltageclampactivation_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclampactivation_la_include_HEADERS = $(HEADER_PATH)/activation.h



check_PROGRAMS = \
    linktest_libvoltageclampactivation_la

linktest_libvoltageclampactivation_la_SOURCES = linktest.cc
linktest_libvoltageclampactivation_la_LDADD = libvoltageclampactivation.la

TESTS = $(check_PROGRAMS)
