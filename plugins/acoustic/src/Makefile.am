HEADER_PATH = ../include/relacs/acoustic
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libacoustictraces.la \
    libacousticloudspeaker.la \
    libacousticcalibspeakers.la \
    libacousticcalibmicrophone.la \
    libacousticechotest.la \
    libacousticsoundlevel.la



libacoustictraces_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacoustictraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacoustictraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(QT4_LIBS) $(GSL_LIBS)

libacoustictraces_la_SOURCES = traces.cc

libacoustictraces_la_includedir = $(pkgincludedir)/acoustic

libacoustictraces_la_include_HEADERS = $(HEADER_PATH)/traces.h



libacousticloudspeaker_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacousticloudspeaker_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacousticloudspeaker_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(QT4_LIBS) $(GSL_LIBS)

libacousticloudspeaker_la_SOURCES = loudspeaker.cc

libacousticloudspeaker_la_includedir = $(pkgincludedir)/acoustic

libacousticloudspeaker_la_include_HEADERS = $(HEADER_PATH)/loudspeaker.h



libacousticcalibspeakers_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacousticcalibspeakers_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacousticcalibspeakers_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/libacoustictraces.la \
    $(builddir)/libacousticloudspeaker.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libacousticcalibspeakers_la_OBJECTS) : moc_calibspeakers.cc

libacousticcalibspeakers_la_SOURCES = calibspeakers.cc

libacousticcalibspeakers_la_includedir = $(pkgincludedir)/acoustic

libacousticcalibspeakers_la_include_HEADERS = $(HEADER_PATH)/calibspeakers.h


libacousticcalibmicrophone_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacousticcalibmicrophone_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacousticcalibmicrophone_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/libacoustictraces.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libacousticcalibmicrophone_la_OBJECTS) : moc_calibmicrophone.cc

libacousticcalibmicrophone_la_SOURCES = calibmicrophone.cc

libacousticcalibmicrophone_la_includedir = $(pkgincludedir)/acoustic

libacousticcalibmicrophone_la_include_HEADERS = $(HEADER_PATH)/calibmicrophone.h



libacousticechotest_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacousticechotest_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacousticechotest_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(builddir)/libacoustictraces.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libacousticechotest_la_OBJECTS) : moc_echotest.cc

libacousticechotest_la_SOURCES = echotest.cc

libacousticechotest_la_includedir = $(pkgincludedir)/acoustic

libacousticechotest_la_include_HEADERS = $(HEADER_PATH)/echotest.h



libacousticsoundlevel_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libacousticsoundlevel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libacousticsoundlevel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libacousticsoundlevel_la_OBJECTS) : moc_soundlevel.cc

libacousticsoundlevel_la_SOURCES = soundlevel.cc

libacousticsoundlevel_la_includedir = $(pkgincludedir)/acoustic

libacousticsoundlevel_la_include_HEADERS = $(HEADER_PATH)/soundlevel.h



check_PROGRAMS = \
    linktest_libacoustictraces_la \
    linktest_libacousticloudspeaker_la \
    linktest_libacousticcalibspeakers_la \
    linktest_libacousticcalibmicrophone_la \
    linktest_libacousticechotest_la \
    linktest_libacousticsoundlevel_la

linktest_libacoustictraces_la_SOURCES = linktest.cc
linktest_libacoustictraces_la_LDADD = libacoustictraces.la

linktest_libacousticloudspeaker_la_SOURCES = linktest.cc
linktest_libacousticloudspeaker_la_LDADD = libacousticloudspeaker.la

linktest_libacousticcalibspeakers_la_SOURCES = linktest.cc
linktest_libacousticcalibspeakers_la_LDADD = libacousticcalibspeakers.la

linktest_libacousticcalibmicrophone_la_SOURCES = linktest.cc
linktest_libacousticcalibmicrophone_la_LDADD = libacousticcalibmicrophone.la

linktest_libacousticechotest_la_SOURCES = linktest.cc
linktest_libacousticechotest_la_LDADD = libacousticechotest.la

linktest_libacousticsoundlevel_la_SOURCES = linktest.cc
linktest_libacousticsoundlevel_la_LDADD = libacousticsoundlevel.la

TESTS = $(check_PROGRAMS)

