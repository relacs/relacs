HEADER_PATH = ../include/relacs/efield
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libefieldeodtools.la \
    libefieldlinearfield.la \
    libefieldeoddetector.la \
    libefieldchirpdetector.la \
    libefieldbeatdetector.la \
    libefieldtraces.la \
    libefieldcalibefield.la \
    libefieldjar.la \
    libefieldmanualjar.la \
    libefieldeodmodel.la \
    libefielddualbeat.la \
    libefieldbeats.la


if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libefieldefieldgeometry.la \
    libefieldcalibraterobot.la
endif



libefieldeodtools_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldeodtools_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldeodtools_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libefieldeodtools_la_SOURCES = eodtools.cc

libefieldeodtools_la_includedir = $(pkgincludedir)/efield

libefieldeodtools_la_include_HEADERS = $(HEADER_PATH)/eodtools.h



libefieldlinearfield_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldlinearfield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldlinearfield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldlinearfield_la_OBJECTS) : moc_linearfield.cc

libefieldlinearfield_la_SOURCES = linearfield.cc

libefieldlinearfield_la_includedir = $(pkgincludedir)/efield

libefieldlinearfield_la_include_HEADERS = $(HEADER_PATH)/linearfield.h




libefieldeoddetector_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldeoddetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldeoddetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldeoddetector_la_OBJECTS) : moc_eoddetector.cc

libefieldeoddetector_la_SOURCES = eoddetector.cc

libefieldeoddetector_la_includedir = $(pkgincludedir)/efield

libefieldeoddetector_la_include_HEADERS = $(HEADER_PATH)/eoddetector.h



libefieldchirpdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldchirpdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldchirpdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldchirpdetector_la_OBJECTS) : moc_chirpdetector.cc

libefieldchirpdetector_la_SOURCES = chirpdetector.cc

libefieldchirpdetector_la_includedir = $(pkgincludedir)/efield

libefieldchirpdetector_la_include_HEADERS = $(HEADER_PATH)/chirpdetector.h



libefieldbeatdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldbeatdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldbeatdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldbeatdetector_la_OBJECTS) : moc_beatdetector.cc

libefieldbeatdetector_la_SOURCES = beatdetector.cc

libefieldbeatdetector_la_includedir = $(pkgincludedir)/efield

libefieldbeatdetector_la_include_HEADERS = $(HEADER_PATH)/beatdetector.h



libefieldtraces_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldtraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldtraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libefieldtraces_la_SOURCES = traces.cc

libefieldtraces_la_includedir = $(pkgincludedir)/efield

libefieldtraces_la_include_HEADERS = $(HEADER_PATH)/traces.h



libefieldcalibefield_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldcalibefield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldcalibefield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldcalibefield_la_OBJECTS) : moc_calibefield.cc

libefieldcalibefield_la_SOURCES = calibefield.cc

libefieldcalibefield_la_includedir = $(pkgincludedir)/efield

libefieldcalibefield_la_include_HEADERS = $(HEADER_PATH)/calibefield.h



libefieldjar_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldjar_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldjar_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldjar_la_OBJECTS) : moc_jar.cc

libefieldjar_la_SOURCES = jar.cc

libefieldjar_la_includedir = $(pkgincludedir)/efield

libefieldjar_la_include_HEADERS = $(HEADER_PATH)/jar.h



libefieldmanualjar_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldmanualjar_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldmanualjar_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldmanualjar_la_OBJECTS) : moc_manualjar.cc

libefieldmanualjar_la_SOURCES = manualjar.cc

libefieldmanualjar_la_includedir = $(pkgincludedir)/efield

libefieldmanualjar_la_include_HEADERS = $(HEADER_PATH)/manualjar.h



libefieldeodmodel_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldeodmodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldeodmodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libefieldeodmodel_la_SOURCES = eodmodel.cc

libefieldeodmodel_la_includedir = $(pkgincludedir)/efield

libefieldeodmodel_la_include_HEADERS = $(HEADER_PATH)/eodmodel.h



libefieldbeats_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefieldbeats_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefieldbeats_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/../../base/src/libbasetraces.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldbeats_la_OBJECTS) : moc_beats.cc

libefieldbeats_la_SOURCES = beats.cc

libefieldbeats_la_includedir = $(pkgincludedir)/efield

libefieldbeats_la_include_HEADERS = $(HEADER_PATH)/beats.h

libefielddualbeat_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

 libefielddualbeat_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefielddualbeat_la_LIBADD = \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefielddualbeat_la_OBJECTS) : moc_dualbeat.cc

libefielddualbeat_la_SOURCES = dualbeat.cc

libefielddualbeat_la_includedir = $(pkgincludedir)/efield

libefielddualbeat_la_include_HEADERS = $(HEADER_PATH)/dualbeat.h


libefieldefieldgeometry_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)$(GSL_CPPFLAGS)

libefieldefieldgeometry_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libefieldefieldgeometry_la_LIBADD = \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscxyzrobot.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldefieldgeometry_la_OBJECTS) : moc_efieldgeometry.cc

libefieldefieldgeometry_la_SOURCES = efieldgeometry.cc

libefieldefieldgeometry_la_includedir = $(pkgincludedir)/efield

libefieldefieldgeometry_la_include_HEADERS = $(HEADER_PATH)/efieldgeometry.h



libefieldcalibraterobot_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(top_srcdir)/plugins/base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)$(GSL_CPPFLAGS)

libefieldcalibraterobot_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libefieldcalibraterobot_la_LIBADD = \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscxyzrobot.la \
    $(top_builddir)/plugins/base/src/libbaserobot.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefieldcalibraterobot_la_OBJECTS) : moc_calibraterobot.cc

libefieldcalibraterobot_la_SOURCES = calibraterobot.cc

libefieldcalibraterobot_la_includedir = $(pkgincludedir)/efield

libefieldcalibraterobot_la_include_HEADERS = $(HEADER_PATH)/calibraterobot.h



check_PROGRAMS = \
    linktest_libefieldeodtools_la \
    linktest_libefieldlinearfield_la \
    linktest_libefieldeoddetector_la \
    linktest_libefieldchirpdetector_la \
    linktest_libefieldbeatdetector_la \
    linktest_libefieldtraces_la \
    linktest_libefieldcalibefield_la \
    linktest_libefieldjar_la \
    linktest_libefieldmanualjar_la \
    linktest_libefieldeodmodel_la \
    linktest_libefieldbeats_la \
    linktest_libefielddualbeat_la

if RELACS_COND_TML
check_PROGRAMS += \
    linktest_libefieldefieldgeometry_la \
    linktest_libefieldcalibraterobot_la
endif


linktest_libefieldeodtools_la_SOURCES = linktest.cc
linktest_libefieldeodtools_la_LDADD = libefieldeodtools.la

linktest_libefieldlinearfield_la_SOURCES = linktest.cc
linktest_libefieldlinearfield_la_LDADD = libefieldlinearfield.la

linktest_libefieldeoddetector_la_SOURCES = linktest.cc
linktest_libefieldeoddetector_la_LDADD = libefieldeoddetector.la

linktest_libefieldchirpdetector_la_SOURCES = linktest.cc
linktest_libefieldchirpdetector_la_LDADD = libefieldchirpdetector.la

linktest_libefieldbeatdetector_la_SOURCES = linktest.cc
linktest_libefieldbeatdetector_la_LDADD = libefieldbeatdetector.la

linktest_libefieldtraces_la_SOURCES = linktest.cc
linktest_libefieldtraces_la_LDADD = libefieldtraces.la

linktest_libefieldcalibefield_la_SOURCES = linktest.cc
linktest_libefieldcalibefield_la_LDADD = libefieldcalibefield.la

linktest_libefieldjar_la_SOURCES = linktest.cc
linktest_libefieldjar_la_LDADD = libefieldjar.la

linktest_libefieldmanualjar_la_SOURCES = linktest.cc
linktest_libefieldmanualjar_la_LDADD = libefieldmanualjar.la

linktest_libefieldeodmodel_la_SOURCES = linktest.cc
linktest_libefieldeodmodel_la_LDADD = libefieldeodmodel.la

linktest_libefieldbeats_la_SOURCES = linktest.cc
linktest_libefieldbeats_la_LDADD = libefieldbeats.la

linktest_libefielddualbeat_la_SOURCES = linktest.cc
linktest_libefielddualbeat_la_LDADD = libefielddualbeat.la

linktest_libefieldefieldgeometry_la_SOURCES = linktest.cc
linktest_libefieldefieldgeometry_la_LDADD = libefieldefieldgeometry.la

linktest_libefieldcalibraterobot_la_SOURCES = linktest.cc
linktest_libefieldcalibraterobot_la_LDADD = libefieldcalibraterobot.la

TESTS = $(check_PROGRAMS)
