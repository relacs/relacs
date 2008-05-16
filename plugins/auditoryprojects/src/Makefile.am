HEADER_PATH = ../include/relacs/auditoryprojects
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libauditoryprojectsspikeprecision.la \
    libauditoryprojectsisoresponse.la \
    libauditoryprojectsoneclick.la



libauditoryprojectsspikeprecision_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/linuxdevices/nieseries/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/auditory/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryprojectsspikeprecision_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryprojectsspikeprecision_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(builddir)/../../auditory/src/libauditorysession.la \
    $(QT3_LIBS)

$(libauditoryprojectsspikeprecision_la_OBJECTS) : moc_spikeprecision.cc

libauditoryprojectsspikeprecision_la_SOURCES = spikeprecision.cc

libauditoryprojectsspikeprecision_la_includedir = $(pkgincludedir)/auditoryprojects

libauditoryprojectsspikeprecision_la_include_HEADERS = $(HEADER_PATH)/spikeprecision.h



libauditoryprojectsisoresponse_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/linuxdevices/nieseries/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/auditory/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryprojectsisoresponse_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryprojectsisoresponse_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(builddir)/../../auditory/src/libauditorysession.la \
    $(QT3_LIBS)

$(libauditoryprojectsisoresponse_la_OBJECTS) : moc_isoresponse.cc

libauditoryprojectsisoresponse_la_SOURCES = isoresponse.cc

libauditoryprojectsisoresponse_la_includedir = $(pkgincludedir)/auditoryprojects

libauditoryprojectsisoresponse_la_include_HEADERS = $(HEADER_PATH)/isoresponse.h



libauditoryprojectsoneclick_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/linuxdevices/nieseries/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(top_srcdir)/plugins/acoustic/include \
    -I$(top_srcdir)/plugins/auditory/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryprojectsoneclick_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryprojectsoneclick_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(builddir)/../../ephys/src/libephystraces.la \
    $(builddir)/../../acoustic/src/libacoustictraces.la \
    $(builddir)/../../acoustic/src/libacousticloudspeaker.la \
    $(QT3_LIBS)

$(libauditoryprojectsoneclick_la_OBJECTS) : moc_oneclick.cc

libauditoryprojectsoneclick_la_SOURCES = oneclick.cc

libauditoryprojectsoneclick_la_includedir = $(pkgincludedir)/auditoryprojects

libauditoryprojectsoneclick_la_include_HEADERS = $(HEADER_PATH)/oneclick.h


check_PROGRAMS = \
    linktest_libauditoryprojectsspikeprecision_la \
    linktest_libauditoryprojectsisoresponse_la \
    linktest_libauditoryprojectsoneclick_la

linktest_libauditoryprojectsspikeprecision_la_SOURCES = linktest.cc
linktest_libauditoryprojectsspikeprecision_la_LDADD = libauditoryprojectsspikeprecision.la

linktest_libauditoryprojectsisoresponse_la_SOURCES = linktest.cc
linktest_libauditoryprojectsisoresponse_la_LDADD = libauditoryprojectsisoresponse.la

linktest_libauditoryprojectsoneclick_la_SOURCES = linktest.cc
linktest_libauditoryprojectsoneclick_la_LDADD = libauditoryprojectsoneclick.la

TESTS = $(check_PROGRAMS)
