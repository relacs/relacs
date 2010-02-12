HEADER_PATH = ../include/relacs/efish
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libefishbaselineactivity.la \
    libefishsession.la \
    libefishpunitmodel.la \
    libefishsam.la



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
    $(QT3_CPPFLAGS)

libefishbaselineactivity_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libefishbaselineactivity_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(QT3_LIBS)

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
    $(QT3_CPPFLAGS)

libefishsession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libefishsession_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(QT3_LIBS)

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
    $(QT3_CPPFLAGS)

libefishpunitmodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libefishpunitmodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/librelacsspikingneuron.la \
    $(top_builddir)/plugins/ephys/src/libephysneuronmodels.la \
    $(top_builddir)/plugins/efield/src/libefieldtraces.la \
    $(QT3_LIBS)

$(libefishpunitmodel_la_OBJECTS) : moc_punitmodel.cc

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
    $(QT3_CPPFLAGS)

libefishsam_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libefishsam_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../efield/src/libefieldtraces.la \
    $(builddir)/../../efield/src/libefieldeodtools.la \
    $(QT3_LIBS)

$(libefishsam_la_OBJECTS) : moc_sam.cc

libefishsam_la_SOURCES = sam.cc

libefishsam_la_includedir = $(pkgincludedir)/efish

libefishsam_la_include_HEADERS = $(HEADER_PATH)/sam.h



check_PROGRAMS = \
    linktest_libefishbaselineactivity_la \
    linktest_libefishsession_la \
    linktest_libefishpunitmodel_la \
    linktest_libefishsam_la

linktest_libefishbaselineactivity_la_SOURCES = linktest.cc
linktest_libefishbaselineactivity_la_LDADD = libefishbaselineactivity.la

linktest_libefishsession_la_SOURCES = linktest.cc
linktest_libefishsession_la_LDADD = libefishsession.la

linktest_libefishpunitmodel_la_SOURCES = linktest.cc
linktest_libefishpunitmodel_la_LDADD = libefishpunitmodel.la

linktest_libefishsam_la_SOURCES = linktest.cc
linktest_libefishsam_la_LDADD = libefishsam.la

TESTS = $(check_PROGRAMS)
