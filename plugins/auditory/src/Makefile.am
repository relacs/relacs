HEADER_PATH = ../include/relacs/auditory
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libauditoryreceptormodel.la \
    libauditorysession.la \
    libauditorysearch.la \
    libauditoryficurve.la \
    libauditoryfifield.la \
    libauditorysinglestimulus.la \
    libauditorysyslatency.la \
    libauditoryadaptedficurves.la


libauditoryreceptormodel_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditoryreceptormodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditoryreceptormodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephysneuronmodels.la \
    $(builddir)/../../ephys/src/librelacsspikingneuron.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libauditoryreceptormodel_la_SOURCES = receptormodel.cc

libauditoryreceptormodel_la_includedir = $(pkgincludedir)/auditory

libauditoryreceptormodel_la_include_HEADERS = $(HEADER_PATH)/receptormodel.h



libauditorysearch_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditorysearch_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditorysearch_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditorysearch_la_OBJECTS) : moc_search.cc

libauditorysearch_la_SOURCES = search.cc

libauditorysearch_la_includedir = $(pkgincludedir)/auditory

libauditorysearch_la_include_HEADERS = $(HEADER_PATH)/search.h



libauditorysession_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditorysession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditorysession_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(top_builddir)/plugins/acoustic/src/libacoustictraces.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditorysession_la_OBJECTS) : moc_session.cc

libauditorysession_la_SOURCES = session.cc

libauditorysession_la_includedir = $(pkgincludedir)/auditory

libauditorysession_la_include_HEADERS = $(HEADER_PATH)/session.h




libauditoryficurve_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditoryficurve_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditoryficurve_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    libauditorysession.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditoryficurve_la_OBJECTS) : moc_ficurve.cc

libauditoryficurve_la_SOURCES = ficurve.cc

libauditoryficurve_la_includedir = $(pkgincludedir)/auditory

libauditoryficurve_la_include_HEADERS = $(HEADER_PATH)/ficurve.h



libauditoryfifield_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditoryfifield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditoryfifield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    libauditoryficurve.la \
    libauditorysession.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditoryfifield_la_OBJECTS) : moc_fifield.cc

libauditoryfifield_la_SOURCES = fifield.cc

libauditoryfifield_la_includedir = $(pkgincludedir)/auditory

libauditoryfifield_la_include_HEADERS = $(HEADER_PATH)/fifield.h



libauditorysinglestimulus_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditorysinglestimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditorysinglestimulus_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    libauditorysession.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditorysinglestimulus_la_OBJECTS) : moc_singlestimulus.cc

libauditorysinglestimulus_la_SOURCES = singlestimulus.cc

libauditorysinglestimulus_la_includedir = $(pkgincludedir)/auditory

libauditorysinglestimulus_la_include_HEADERS = $(HEADER_PATH)/singlestimulus.h




libauditorysyslatency_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditorysyslatency_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditorysyslatency_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    libauditorysession.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditorysyslatency_la_OBJECTS) : moc_syslatency.cc

libauditorysyslatency_la_SOURCES = syslatency.cc

libauditorysyslatency_la_includedir = $(pkgincludedir)/auditory

libauditorysyslatency_la_include_HEADERS = $(HEADER_PATH)/syslatency.h



libauditoryadaptedficurves_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libauditoryadaptedficurves_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libauditoryadaptedficurves_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libauditoryadaptedficurves_la_OBJECTS) : moc_adaptedficurves.cc

libauditoryadaptedficurves_la_SOURCES = adaptedficurves.cc

libauditoryadaptedficurves_la_includedir = $(pkgincludedir)/auditory

libauditoryadaptedficurves_la_include_HEADERS = $(HEADER_PATH)/adaptedficurves.h



check_PROGRAMS = \
    linktest_libauditoryreceptormodel_la \
    linktest_libauditorysearch_la \
    linktest_libauditorysession_la \
    linktest_libauditoryficurve_la \
    linktest_libauditoryfifield_la \
    linktest_libauditorysinglestimulus_la \
    linktest_libauditorysyslatency_la \
    linktest_libauditoryadaptedficurves_la

linktest_libauditoryreceptormodel_la_SOURCES = linktest.cc
linktest_libauditoryreceptormodel_la_LDADD = libauditoryreceptormodel.la

linktest_libauditorysearch_la_SOURCES = linktest.cc
linktest_libauditorysearch_la_LDADD = libauditorysearch.la

linktest_libauditorysession_la_SOURCES = linktest.cc
linktest_libauditorysession_la_LDADD = libauditorysession.la

linktest_libauditoryficurve_la_SOURCES = linktest.cc
linktest_libauditoryficurve_la_LDADD = libauditoryficurve.la

linktest_libauditoryfifield_la_SOURCES = linktest.cc
linktest_libauditoryfifield_la_LDADD = libauditoryfifield.la

linktest_libauditorysinglestimulus_la_SOURCES = linktest.cc
linktest_libauditorysinglestimulus_la_LDADD = libauditorysinglestimulus.la

linktest_libauditorysyslatency_la_SOURCES = linktest.cc
linktest_libauditorysyslatency_la_LDADD = libauditorysyslatency.la

linktest_libauditoryadaptedficurves_la_SOURCES = linktest.cc
linktest_libauditoryadaptedficurves_la_LDADD = libauditoryadaptedficurves.la

TESTS = $(check_PROGRAMS)

