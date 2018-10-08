HEADER_PATH = ../include/relacs/voltageclamp
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libvoltageclampactivation.la \
    libvoltageclampinactivation.la \
    libvoltageclamprecovery.la \
    libvoltageclampramp.la \
    libvoltageclamptail.la \
    libvoltageclampmembranetest.la \
    libvoltageclampsummary.la \
    libvoltageclamppnsubtraction.la


libvoltageclamppnsubtraction_la_CPPFLAGS = \
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

libvoltageclamppnsubtraction_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclamppnsubtraction_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libvoltageclamppnsubtraction_la_OBJECTS) : moc_pnsubtraction.cc

libvoltageclamppnsubtraction_la_SOURCES = pnsubtraction.cc

libvoltageclamppnsubtraction_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclamppnsubtraction_la_include_HEADERS = $(HEADER_PATH)/pnsubtraction.h

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



libvoltageclampinactivation_la_CPPFLAGS = \
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

libvoltageclampinactivation_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclampinactivation_la_LIBADD = \
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

$(libvoltageclampinactivation_la_OBJECTS) : moc_inactivation.cc

libvoltageclampinactivation_la_SOURCES = inactivation.cc

libvoltageclampinactivation_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclampinactivation_la_include_HEADERS = $(HEADER_PATH)/inactivation.h



libvoltageclamprecovery_la_CPPFLAGS = \
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

libvoltageclamprecovery_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclamprecovery_la_LIBADD = \
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

$(libvoltageclamprecovery_la_OBJECTS) : moc_recovery.cc

libvoltageclamprecovery_la_SOURCES = recovery.cc

libvoltageclamprecovery_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclamprecovery_la_include_HEADERS = $(HEADER_PATH)/recovery.h



libvoltageclampramp_la_CPPFLAGS = \
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

libvoltageclampramp_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclampramp_la_LIBADD = \
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

$(libvoltageclampramp_la_OBJECTS) : moc_ramp.cc

libvoltageclampramp_la_SOURCES = ramp.cc

libvoltageclampramp_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclampramp_la_include_HEADERS = $(HEADER_PATH)/ramp.h



libvoltageclamptail_la_CPPFLAGS = \
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

libvoltageclamptail_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclamptail_la_LIBADD = \
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

$(libvoltageclamptail_la_OBJECTS) : moc_tail.cc

libvoltageclamptail_la_SOURCES = tail.cc

libvoltageclamptail_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclamptail_la_include_HEADERS = $(HEADER_PATH)/tail.h



libvoltageclampmembranetest_la_CPPFLAGS = \
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

libvoltageclampmembranetest_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclampmembranetest_la_LIBADD = \
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

$(libvoltageclampmembranetest_la_OBJECTS) : moc_membranetest.cc

libvoltageclampmembranetest_la_SOURCES = membranetest.cc

libvoltageclampmembranetest_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclampmembranetest_la_include_HEADERS = $(HEADER_PATH)/membranetest.h



libvoltageclampsummary_la_CPPFLAGS = \
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

libvoltageclampsummary_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libvoltageclampsummary_la_LIBADD = \
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

$(libvoltageclampsummary_la_OBJECTS) : moc_summary.cc

libvoltageclampsummary_la_SOURCES = summary.cc

libvoltageclampsummary_la_includedir = $(pkgincludedir)/voltageclamp

libvoltageclampsummary_la_include_HEADERS = $(HEADER_PATH)/summary.h






check_PROGRAMS = \
    linktest_libvoltageclampactivation_la \
    linktest_libvoltageclampinactivation_la \
    linktest_libvoltageclamprecovery_la \
    linktest_libvoltageclampramp_la \
    linktest_libvoltageclamptail_la \
    linktest_libvoltageclampmembranetest_la \
    linktest_libvoltageclampsummary_la \
    linktest_libvoltageclamppnsubtraction_la

linktest_libvoltageclampactivation_la_SOURCES = linktest.cc
linktest_libvoltageclampactivation_la_LDADD = libvoltageclampactivation.la

linktest_libvoltageclampinactivation_la_SOURCES = linktest.cc
linktest_libvoltageclampinactivation_la_LDADD = libvoltageclampinactivation.la

linktest_libvoltageclamprecovery_la_SOURCES = linktest.cc
linktest_libvoltageclamprecovery_la_LDADD = libvoltageclamprecovery.la

linktest_libvoltageclampramp_la_SOURCES = linktest.cc
linktest_libvoltageclampramp_la_LDADD = libvoltageclampramp.la

linktest_libvoltageclamptail_la_SOURCES = linktest.cc
linktest_libvoltageclamptail_la_LDADD = libvoltageclamptail.la

linktest_libvoltageclampmembranetest_la_SOURCES = linktest.cc
linktest_libvoltageclampmembranetest_la_LDADD = libvoltageclampmembranetest.la

linktest_libvoltageclampsummary_la_SOURCES = linktest.cc
linktest_libvoltageclampsummary_la_LDADD = libvoltageclampsummary.la

linktest_libvoltageclamppnsubtraction_la_SOURCES = linktest.cc
linktest_libvoltageclamppnsubtraction_la_LDADD = libvoltageclamppnsubtraction.la

TESTS = $(check_PROGRAMS)
