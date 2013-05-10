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
    libefieldbeats.la



libefieldeodtools_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldeodtools_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldeodtools_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

libefieldeodtools_la_SOURCES = eodtools.cc

libefieldeodtools_la_includedir = $(pkgincludedir)/efield

libefieldeodtools_la_include_HEADERS = $(HEADER_PATH)/eodtools.h



libefieldlinearfield_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldlinearfield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldlinearfield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldlinearfield_la_OBJECTS) : moc_linearfield.cc

libefieldlinearfield_la_SOURCES = linearfield.cc

libefieldlinearfield_la_includedir = $(pkgincludedir)/efield

libefieldlinearfield_la_include_HEADERS = $(HEADER_PATH)/linearfield.h




libefieldeoddetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldeoddetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldeoddetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldeoddetector_la_OBJECTS) : moc_eoddetector.cc

libefieldeoddetector_la_SOURCES = eoddetector.cc

libefieldeoddetector_la_includedir = $(pkgincludedir)/efield

libefieldeoddetector_la_include_HEADERS = $(HEADER_PATH)/eoddetector.h



libefieldchirpdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldchirpdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldchirpdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldchirpdetector_la_OBJECTS) : moc_chirpdetector.cc

libefieldchirpdetector_la_SOURCES = chirpdetector.cc

libefieldchirpdetector_la_includedir = $(pkgincludedir)/efield

libefieldchirpdetector_la_include_HEADERS = $(HEADER_PATH)/chirpdetector.h



libefieldbeatdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldbeatdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldbeatdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldbeatdetector_la_OBJECTS) : moc_beatdetector.cc

libefieldbeatdetector_la_SOURCES = beatdetector.cc

libefieldbeatdetector_la_includedir = $(pkgincludedir)/efield

libefieldbeatdetector_la_include_HEADERS = $(HEADER_PATH)/beatdetector.h



libefieldtraces_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldtraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldtraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

libefieldtraces_la_SOURCES = traces.cc

libefieldtraces_la_includedir = $(pkgincludedir)/efield

libefieldtraces_la_include_HEADERS = $(HEADER_PATH)/traces.h



libefieldcalibefield_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldcalibefield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldcalibefield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldcalibefield_la_OBJECTS) : moc_calibefield.cc

libefieldcalibefield_la_SOURCES = calibefield.cc

libefieldcalibefield_la_includedir = $(pkgincludedir)/efield

libefieldcalibefield_la_include_HEADERS = $(HEADER_PATH)/calibefield.h



libefieldjar_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldjar_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldjar_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldjar_la_OBJECTS) : moc_jar.cc

libefieldjar_la_SOURCES = jar.cc

libefieldjar_la_includedir = $(pkgincludedir)/efield

libefieldjar_la_include_HEADERS = $(HEADER_PATH)/jar.h



libefieldmanualjar_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldmanualjar_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldmanualjar_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldmanualjar_la_OBJECTS) : moc_manualjar.cc

libefieldmanualjar_la_SOURCES = manualjar.cc

libefieldmanualjar_la_includedir = $(pkgincludedir)/efield

libefieldmanualjar_la_include_HEADERS = $(HEADER_PATH)/manualjar.h



libefieldeodmodel_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldeodmodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldeodmodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldeodmodel_la_OBJECTS) : moc_eodmodel.cc

libefieldeodmodel_la_SOURCES = eodmodel.cc

libefieldeodmodel_la_includedir = $(pkgincludedir)/efield

libefieldeodmodel_la_include_HEADERS = $(HEADER_PATH)/eodmodel.h



libefieldbeats_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldbeats_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldbeats_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbaselinearattenuate.la \
    $(builddir)/libefieldtraces.la \
    $(builddir)/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libefieldbeats_la_OBJECTS) : moc_beats.cc

libefieldbeats_la_SOURCES = beats.cc

libefieldbeats_la_includedir = $(pkgincludedir)/efield

libefieldbeats_la_include_HEADERS = $(HEADER_PATH)/beats.h



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
    linktest_libefieldbeats_la

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

TESTS = $(check_PROGRAMS)
