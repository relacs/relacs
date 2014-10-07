HEADER_PATH = ../include/relacs/ephys
include ${top_srcdir}/moc4.mk

lib_LTLIBRARIES = \
    librelacsspikingneuron.la


pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libephystraces.la \
    libephysneuronmodels.la \
    libephysintraspikedetector.la \
    libephysdynamicsuspikedetector.la \
    libephysthresholdsuspikedetector.la \
    libephysmotorcontrol.la \
    libephysamplifiercontrol.la \
    libephyscapacitycompensation.la \
    libephysbridgetest.la \
    libephysiontophoresis.la



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
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephystraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephystraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

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
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

libephysneuronmodels_la_SOURCES = neuronmodels.cc

libephysneuronmodels_la_includedir = $(pkgincludedir)/ephys

libephysneuronmodels_la_include_HEADERS = $(HEADER_PATH)/neuronmodels.h



libephysintraspikedetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysintraspikedetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysintraspikedetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysintraspikedetector_la_OBJECTS) : moc_intraspikedetector.cc

libephysintraspikedetector_la_SOURCES = intraspikedetector.cc

libephysintraspikedetector_la_includedir = $(pkgincludedir)/ephys

libephysintraspikedetector_la_include_HEADERS = $(HEADER_PATH)/intraspikedetector.h



libephysdynamicsuspikedetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysdynamicsuspikedetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysdynamicsuspikedetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysdynamicsuspikedetector_la_OBJECTS) : moc_dynamicsuspikedetector.cc

libephysdynamicsuspikedetector_la_SOURCES = dynamicsuspikedetector.cc

libephysdynamicsuspikedetector_la_includedir = $(pkgincludedir)/ephys

libephysdynamicsuspikedetector_la_include_HEADERS = $(HEADER_PATH)/dynamicsuspikedetector.h



libephysthresholdsuspikedetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysthresholdsuspikedetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysthresholdsuspikedetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysthresholdsuspikedetector_la_OBJECTS) : moc_thresholdsuspikedetector.cc

libephysthresholdsuspikedetector_la_SOURCES = thresholdsuspikedetector.cc

libephysthresholdsuspikedetector_la_includedir = $(pkgincludedir)/ephys

libephysthresholdsuspikedetector_la_include_HEADERS = $(HEADER_PATH)/thresholdsuspikedetector.h



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
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysmotorcontrol_la_OBJECTS) : moc_motorcontrol.cc

libephysmotorcontrol_la_SOURCES = motorcontrol.cc

libephysmotorcontrol_la_includedir = $(pkgincludedir)/ephys

libephysmotorcontrol_la_include_HEADERS = $(HEADER_PATH)/motorcontrol.h



libephysamplifiercontrol_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysamplifiercontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysamplifiercontrol_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscamplmode.la \
    $(builddir)/libephystraces.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysamplifiercontrol_la_OBJECTS) : moc_amplifiercontrol.cc

libephysamplifiercontrol_la_SOURCES = amplifiercontrol.cc

libephysamplifiercontrol_la_includedir = $(pkgincludedir)/ephys

libephysamplifiercontrol_la_include_HEADERS = $(HEADER_PATH)/amplifiercontrol.h




libephyscapacitycompensation_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephyscapacitycompensation_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephyscapacitycompensation_la_LIBADD = \
    $(builddir)/libephystraces.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephyscapacitycompensation_la_OBJECTS) : moc_capacitycompensation.cc

libephyscapacitycompensation_la_SOURCES = capacitycompensation.cc

libephyscapacitycompensation_la_includedir = $(pkgincludedir)/ephys

libephyscapacitycompensation_la_include_HEADERS = $(HEADER_PATH)/capacitycompensation.h



libephysbridgetest_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysbridgetest_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysbridgetest_la_LIBADD = \
    $(builddir)/libephystraces.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysbridgetest_la_OBJECTS) : moc_bridgetest.cc

libephysbridgetest_la_SOURCES = bridgetest.cc

libephysbridgetest_la_includedir = $(pkgincludedir)/ephys

libephysbridgetest_la_include_HEADERS = $(HEADER_PATH)/bridgetest.h



libephysiontophoresis_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libephysiontophoresis_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libephysiontophoresis_la_LIBADD = \
    $(builddir)/libephystraces.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libephysiontophoresis_la_OBJECTS) : moc_iontophoresis.cc

libephysiontophoresis_la_SOURCES = iontophoresis.cc

libephysiontophoresis_la_includedir = $(pkgincludedir)/ephys

libephysiontophoresis_la_include_HEADERS = $(HEADER_PATH)/iontophoresis.h



check_PROGRAMS = \
    linktest_librelacsspikingneuron_la \
    linktest_libephystraces_la \
    linktest_libephysneuronmodels_la \
    linktest_libephysintraspikedetector_la \
    linktest_libephysdynamicsuspikedetector_la \
    linktest_libephysthresholdsuspikedetector_la \
    linktest_libephysmotorcontrol_la \
    linktest_libephysamplifiercontrol_la \
    linktest_libephyscapacitycompensation_la \
    linktest_libephysbridgetest_la \
    linktest_libephysiontophoresis_la

linktest_librelacsspikingneuron_la_SOURCES = linktest.cc
linktest_librelacsspikingneuron_la_LDADD = librelacsspikingneuron.la

linktest_libephystraces_la_SOURCES = linktest.cc
linktest_libephystraces_la_LDADD = libephystraces.la

linktest_libephysneuronmodels_la_SOURCES = linktest.cc
linktest_libephysneuronmodels_la_LDADD = libephysneuronmodels.la

linktest_libephysintraspikedetector_la_SOURCES = linktest.cc
linktest_libephysintraspikedetector_la_LDADD = libephysintraspikedetector.la

linktest_libephysdynamicsuspikedetector_la_SOURCES = linktest.cc
linktest_libephysdynamicsuspikedetector_la_LDADD = libephysdynamicsuspikedetector.la

linktest_libephysthresholdsuspikedetector_la_SOURCES = linktest.cc
linktest_libephysthresholdsuspikedetector_la_LDADD = libephysthresholdsuspikedetector.la

linktest_libephysmotorcontrol_la_SOURCES = linktest.cc
linktest_libephysmotorcontrol_la_LDADD = libephysmotorcontrol.la

linktest_libephysamplifiercontrol_la_SOURCES = linktest.cc
linktest_libephysamplifiercontrol_la_LDADD = libephysamplifiercontrol.la

linktest_libephyscapacitycompensation_la_SOURCES = linktest.cc
linktest_libephyscapacitycompensation_la_LDADD = libephyscapacitycompensation.la

linktest_libephysbridgetest_la_SOURCES = linktest.cc
linktest_libephysbridgetest_la_LDADD = libephysbridgetest.la

linktest_libephysiontophoresis_la_SOURCES = linktest.cc
linktest_libephysiontophoresis_la_LDADD = libephysiontophoresis.la

TESTS = $(check_PROGRAMS)
