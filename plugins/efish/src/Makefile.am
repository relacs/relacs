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
    $(QT4_CPPFLAGS)

libefishbaselineactivity_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishbaselineactivity_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(QT4_LIBS)

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
    -I$(srcdir)/../../ephys/include \
    -I$(srcdir)/../../efield/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefishsession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishsession_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(QT4_LIBS)

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
    $(QT4_CPPFLAGS)

libefishpunitmodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishpunitmodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/librelacsspikingneuron.la \
    $(top_builddir)/plugins/ephys/src/libephysneuronmodels.la \
    $(top_builddir)/plugins/efield/src/libefieldtraces.la \
    $(QT4_LIBS)

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
    $(QT4_CPPFLAGS)

libefishsam_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishsam_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(QT4_LIBS)

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
    $(QT4_CPPFLAGS)

libefishficurve_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishficurve_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(QT4_LIBS)

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
    $(QT4_CPPFLAGS)

libefishfilestimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishfilestimulus_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(QT4_LIBS)

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
    $(QT4_CPPFLAGS)

libefishchirps_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefishchirps_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(QT4_LIBS)

$(libefishchirps_la_OBJECTS) : moc_chirps.cc

libefishchirps_la_SOURCES = chirps.cc

libefishchirps_la_includedir = $(pkgincludedir)/efish

libefishchirps_la_include_HEADERS = $(HEADER_PATH)/chirps.h



check_PROGRAMS = \
    linktest_libefishbaselineactivity_la \
    linktest_libefishsession_la \
    linktest_libefishpunitmodel_la \
    linktest_libefishsam_la \
    linktest_libefishficurve_la \
    linktest_libefishfilestimulus_la \
    linktest_libefishchirps_la

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

TESTS = $(check_PROGRAMS)
