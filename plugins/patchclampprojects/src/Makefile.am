HEADER_PATH = ../include/relacs/patchclampprojects
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libpatchclampprojectsthresholdlatencies.la \
    libpatchclampprojectsfindthreshold.la \
    libpatchclampprojectssetleak.la \
    libpatchclampprojectsvoltagereconstruction.la \
    libpatchclampprojectsfeedforwardnetwork.la



libpatchclampprojectsthresholdlatencies_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libpatchclampprojectsthresholdlatencies_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libpatchclampprojectsthresholdlatencies_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT4_LIBS)

$(libpatchclampprojectsthresholdlatencies_la_OBJECTS) : moc_thresholdlatencies.cc

libpatchclampprojectsthresholdlatencies_la_SOURCES = thresholdlatencies.cc

libpatchclampprojectsthresholdlatencies_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectsthresholdlatencies_la_include_HEADERS = $(HEADER_PATH)/thresholdlatencies.h



libpatchclampprojectsfindthreshold_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libpatchclampprojectsfindthreshold_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libpatchclampprojectsfindthreshold_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT4_LIBS)

$(libpatchclampprojectsfindthreshold_la_OBJECTS) : moc_findthreshold.cc

libpatchclampprojectsfindthreshold_la_SOURCES = findthreshold.cc

libpatchclampprojectsfindthreshold_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectsfindthreshold_la_include_HEADERS = $(HEADER_PATH)/findthreshold.h



libpatchclampprojectssetleak_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libpatchclampprojectssetleak_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libpatchclampprojectssetleak_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libpatchclampprojectssetleak_la_OBJECTS) : moc_setleak.cc

libpatchclampprojectssetleak_la_SOURCES = setleak.cc

libpatchclampprojectssetleak_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectssetleak_la_include_HEADERS = $(HEADER_PATH)/setleak.h



libpatchclampprojectsvoltagereconstruction_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libpatchclampprojectsvoltagereconstruction_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libpatchclampprojectsvoltagereconstruction_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT4_LIBS)

$(libpatchclampprojectsvoltagereconstruction_la_OBJECTS) : moc_voltagereconstruction.cc

libpatchclampprojectsvoltagereconstruction_la_SOURCES = voltagereconstruction.cc

libpatchclampprojectsvoltagereconstruction_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectsvoltagereconstruction_la_include_HEADERS = $(HEADER_PATH)/voltagereconstruction.h



libpatchclampprojectsfeedforwardnetwork_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libpatchclampprojectsfeedforwardnetwork_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libpatchclampprojectsfeedforwardnetwork_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libpatchclampprojectsfeedforwardnetwork_la_OBJECTS) : moc_feedforwardnetwork.cc

libpatchclampprojectsfeedforwardnetwork_la_SOURCES = feedforwardnetwork.cc

libpatchclampprojectsfeedforwardnetwork_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectsfeedforwardnetwork_la_include_HEADERS = $(HEADER_PATH)/feedforwardnetwork.h



check_PROGRAMS = \
    linktest_libpatchclampprojectsthresholdlatencies_la \
    linktest_libpatchclampprojectsfindthreshold_la \
    linktest_libpatchclampprojectssetleak_la \
    linktest_libpatchclampprojectsvoltagereconstruction_la \
    linktest_libpatchclampprojectsfeedforwardnetwork_la

linktest_libpatchclampprojectsthresholdlatencies_la_SOURCES = linktest.cc
linktest_libpatchclampprojectsthresholdlatencies_la_LDADD = libpatchclampprojectsthresholdlatencies.la

linktest_libpatchclampprojectsfindthreshold_la_SOURCES = linktest.cc
linktest_libpatchclampprojectsfindthreshold_la_LDADD = libpatchclampprojectsfindthreshold.la

linktest_libpatchclampprojectssetleak_la_SOURCES = linktest.cc
linktest_libpatchclampprojectssetleak_la_LDADD = libpatchclampprojectssetleak.la

linktest_libpatchclampprojectsvoltagereconstruction_la_SOURCES = linktest.cc
linktest_libpatchclampprojectsvoltagereconstruction_la_LDADD = libpatchclampprojectsvoltagereconstruction.la

linktest_libpatchclampprojectsfeedforwardnetwork_la_SOURCES = linktest.cc
linktest_libpatchclampprojectsfeedforwardnetwork_la_LDADD = libpatchclampprojectsfeedforwardnetwork.la

TESTS = $(check_PROGRAMS)
