HEADER_PATH = ../include/relacs/patchclampprojects
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libpatchclampprojectsthresholdlatencies.la



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
    $(QT3_CPPFLAGS)

libpatchclampprojectsthresholdlatencies_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libpatchclampprojectsthresholdlatencies_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LIBS)

$(libpatchclampprojectsthresholdlatencies_la_OBJECTS) : moc_thresholdlatencies.cc

libpatchclampprojectsthresholdlatencies_la_SOURCES = thresholdlatencies.cc

libpatchclampprojectsthresholdlatencies_la_includedir = $(pkgincludedir)/patchclampprojects

libpatchclampprojectsthresholdlatencies_la_include_HEADERS = $(HEADER_PATH)/thresholdlatencies.h



check_PROGRAMS = \
    linktest_libpatchclampprojectsthresholdlatencies_la

linktest_libpatchclampprojectsthresholdlatencies_la_SOURCES = linktest.cc
linktest_libpatchclampprojectsthresholdlatencies_la_LDADD = libpatchclampprojectsthresholdlatencies.la

TESTS = $(check_PROGRAMS)
