HEADER_PATH = ../include/relacs/ephys
include ${top_srcdir}/moc4.mk

lib_LTLIBRARIES = \
    librelacsspikingneuron.la


pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libephystraces.la \
    libephysneuronmodels.la \
    libephysspikedetector.la \
    libephysmotorcontrol.la



librelacsspikingneuron_la_CPPFLAGS = \
    -I$(top_srcdir)/options/include \
    -I$(srcdir)/../include

librelacsspikingneuron_la_LDFLAGS = -version-info=0:0:0

librelacsspikingneuron_la_LIBADD = $(top_builddir)/options/src/librelacsoptions.la

librelacsspikingneuron_la_SOURCES = spikingneuron.cc

librelacsspikingneuron_la_includedir = $(pkgincludedir)

librelacsspikingneuron_la_include_HEADERS = ../include/relacs/spikingneuron.h



libephystraces_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(srcdir)/../include

libephystraces_la_LDFLAGS = -module -avoid-version

libephystraces_la_LIBADD = $(top_builddir)/relacs/src/librelacs.la

libephystraces_la_SOURCES = traces.cc

libephystraces_la_includedir = $(pkgincludedir)/ephys

libephystraces_la_include_HEADERS = $(HEADER_PATH)/traces.h




libephysneuronmodels_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysneuronmodels_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysneuronmodels_la_LIBADD = \
    librelacsspikingneuron.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

libephysneuronmodels_la_SOURCES = neuronmodels.cc

libephysneuronmodels_la_includedir = $(pkgincludedir)/ephys

libephysneuronmodels_la_include_HEADERS = $(HEADER_PATH)/neuronmodels.h



libephysspikedetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysspikedetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysspikedetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libephysspikedetector_la_OBJECTS) : moc_spikedetector.cc

libephysspikedetector_la_SOURCES = spikedetector.cc

libephysspikedetector_la_includedir = $(pkgincludedir)/ephys

libephysspikedetector_la_include_HEADERS = $(HEADER_PATH)/spikedetector.h



libephysmotorcontrol_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysmotorcontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysmotorcontrol_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libephysmotorcontrol_la_OBJECTS) : moc_motorcontrol.cc

libephysmotorcontrol_la_SOURCES = motorcontrol.cc

libephysmotorcontrol_la_includedir = $(pkgincludedir)/ephys

libephysmotorcontrol_la_include_HEADERS = $(HEADER_PATH)/motorcontrol.h



check_PROGRAMS = \
    linktest_librelacsspikingneuron_la \
    linktest_libephystraces_la \
    linktest_libephysneuronmodels_la \
    linktest_libephysspikedetector_la \
    linktest_libephysmotorcontrol_la

linktest_librelacsspikingneuron_la_SOURCES = linktest.cc
linktest_librelacsspikingneuron_la_LDADD = librelacsspikingneuron.la

linktest_libephystraces_la_SOURCES = linktest.cc
linktest_libephystraces_la_LDADD = libephystraces.la

linktest_libephysneuronmodels_la_SOURCES = linktest.cc
linktest_libephysneuronmodels_la_LDADD = libephysneuronmodels.la

linktest_libephysspikedetector_la_SOURCES = linktest.cc
linktest_libephysspikedetector_la_LDADD = libephysspikedetector.la

linktest_libephysmotorcontrol_la_SOURCES = linktest.cc
linktest_libephysmotorcontrol_la_LDADD = libephysmotorcontrol.la

TESTS = $(check_PROGRAMS)
