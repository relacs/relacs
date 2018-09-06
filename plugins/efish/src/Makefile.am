HEADER_PATH = ../include/relacs/efish
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libefishbaselineactivity.la \
    libefishsession.la \
    libefishpunitmodel.la \
    libefishsam.la \
    libefishficurve.la \
    libefishfilestimulus.la \
    libefishchirps.la

if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libefishreceptivefield.la \
    libefishrobottofishposition.la
endif


libefishbaselineactivity_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishbaselineactivity_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishbaselineactivity_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishbaselineactivity_la_OBJECTS) : moc_baselineactivity.cc

libefishbaselineactivity_la_SOURCES = baselineactivity.cc

libefishbaselineactivity_la_includedir = $(pkgincludedir)/efish

libefishbaselineactivity_la_include_HEADERS = $(HEADER_PATH)/baselineactivity.h



libefishsession_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../base/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishsession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishsession_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../base/src/libbasetraces.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishsession_la_OBJECTS) : moc_session.cc

libefishsession_la_SOURCES = session.cc

libefishsession_la_includedir = $(pkgincludedir)/efish

libefishsession_la_include_HEADERS = $(HEADER_PATH)/session.h



libefishpunitmodel_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishpunitmodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishpunitmodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(top_builddir)/plugins/ephys/src/librelacsspikingneuron.la \
    $(top_builddir)/plugins/ephys/src/libephysneuronmodels.la \
    $(top_builddir)/plugins/efield/src/libefieldtraces.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libefishpunitmodel_la_SOURCES = punitmodel.cc

libefishpunitmodel_la_includedir = $(pkgincludedir)/efish

libefishpunitmodel_la_include_HEADERS = $(HEADER_PATH)/punitmodel.h



libefishsam_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishsam_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishsam_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishsam_la_OBJECTS) : moc_sam.cc

libefishsam_la_SOURCES = sam.cc

libefishsam_la_includedir = $(pkgincludedir)/efish

libefishsam_la_include_HEADERS = $(HEADER_PATH)/sam.h



libefishficurve_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishficurve_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishficurve_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishficurve_la_OBJECTS) : moc_ficurve.cc

libefishficurve_la_SOURCES = ficurve.cc

libefishficurve_la_includedir = $(pkgincludedir)/efish

libefishficurve_la_include_HEADERS = $(HEADER_PATH)/ficurve.h



libefishfilestimulus_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishfilestimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishfilestimulus_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishfilestimulus_la_OBJECTS) : moc_filestimulus.cc

libefishfilestimulus_la_SOURCES = filestimulus.cc

libefishfilestimulus_la_includedir = $(pkgincludedir)/efish

libefishfilestimulus_la_include_HEADERS = $(HEADER_PATH)/filestimulus.h



libefishchirps_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

libefishchirps_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS)

libefishchirps_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishchirps_la_OBJECTS) : moc_chirps.cc

libefishchirps_la_SOURCES = chirps.cc

libefishchirps_la_includedir = $(pkgincludedir)/efish

libefishchirps_la_include_HEADERS = $(HEADER_PATH)/chirps.h



libefishreceptivefield_la_CPPFLAGS = \
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

libefishreceptivefield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libefishreceptivefield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscxyzrobot.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishreceptivefield_la_OBJECTS) : moc_receptivefield.cc

libefishreceptivefield_la_SOURCES = receptivefield.cc

libefishreceptivefield_la_includedir = $(pkgincludedir)/efish

libefishreceptivefield_la_include_HEADERS = $(HEADER_PATH)/receptivefield.h


libefishrobottofishposition_la_CPPFLAGS = \
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

libefishrobottofishposition_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libefishrobottofishposition_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscxyzrobot.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishrobottofishposition_la_OBJECTS) : moc_robottofishposition.cc

libefishrobottofishposition_la_SOURCES = robottofishposition.cc

libefishrobottofishposition_la_includedir = $(pkgincludedir)/efish

libefishrobottofishposition_la_include_HEADERS = $(HEADER_PATH)/robottofishposition.h




libefishrobottofishposition_la_CPPFLAGS = \
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

libefishrobottofishposition_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) $(NIX_LDFLAGS) $(GSL_LDFLAGS)

libefishrobottofishposition_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscxyzrobot.la \
    $(QT4_LIBS) $(NIX_LIBS) $(GSL_LIBS)

$(libefishrobottofishposition_la_OBJECTS) : moc_robottofishposition.cc

libefishrobottofishposition_la_SOURCES = robottofishposition.cc

libefishrobottofishposition_la_includedir = $(pkgincludedir)/efish

libefishrobottofishposition_la_include_HEADERS = $(HEADER_PATH)/robottofishposition.h



check_PROGRAMS = \
    linktest_libefishbaselineactivity_la \
    linktest_libefishsession_la \
    linktest_libefishpunitmodel_la \
    linktest_libefishsam_la \
    linktest_libefishficurve_la \
    linktest_libefishfilestimulus_la \
    linktest_libefishchirps_la \
    linktest_libefishrobottofishposition_la

if RELACS_COND_TML
check_PROGRAMS += \
    linktest_libefishreceptivefield_la \
    linktest_libefishchirps_la \
    linktest_libefishrobottofishposition_la
    
endif

linktest_libefishbaselineactivity_la_SOURCES = linktest.cc
linktest_libefishbaselineactivity_la_LDADD = libefishbaselineactivity.la

linktest_libefishsession_la_SOURCES = linktest.cc
linktest_libefishsession_la_LDADD = libefishsession.la

linktest_libefishpunitmodel_la_SOURCES = linktest.cc
linktest_libefishpunitmodel_la_LDADD = libefishpunitmodel.la

linktest_libefishsam_la_SOURCES = linktest.cc
linktest_libefishsam_la_LDADD = libefishsam.la

linktest_libefishficurve_la_SOURCES = linktest.cc
linktest_libefishficurve_la_LDADD = libefishficurve.la

linktest_libefishfilestimulus_la_SOURCES = linktest.cc
linktest_libefishfilestimulus_la_LDADD = libefishfilestimulus.la

linktest_libefishchirps_la_SOURCES = linktest.cc
linktest_libefishchirps_la_LDADD = libefishchirps.la

linktest_libefishreceptivefield_la_SOURCES = linktest.cc
linktest_libefishreceptivefield_la_LDADD = libefishreceptivefield.la

linktest_libefishrobottofishposition_la_SOURCES = linktest.cc
linktest_libefishrobottofishposition_la_LDADD = libefishrobottofishposition.la

TESTS = $(check_PROGRAMS)
