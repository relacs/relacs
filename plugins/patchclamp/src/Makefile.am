HEADER_PATH = ../include/relacs/patchclamp
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libpatchclampsession.la \
    libpatchclampsimple.la \
    libpatchclampsinglestimulus.la \
    libpatchclampmembraneresistance.la \
    libpatchclampsetdc.la



libpatchclampsession_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libpatchclampsession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libpatchclampsession_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LIBS)

$(libpatchclampsession_la_OBJECTS) : moc_session.cc

libpatchclampsession_la_SOURCES = session.cc

libpatchclampsession_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampsession_la_include_HEADERS = $(HEADER_PATH)/session.h



libpatchclampsimple_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/base/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libpatchclampsimple_la_LDFLAGS = \
    -module -avoid-version \
    $(top_builddir)/plugins/base/src/libbasesetoutput.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LDFLAGS)

libpatchclampsimple_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libpatchclampsimple_la_OBJECTS) : moc_simple.cc

libpatchclampsimple_la_SOURCES = simple.cc

libpatchclampsimple_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampsimple_la_include_HEADERS = $(HEADER_PATH)/simple.h



libpatchclampsinglestimulus_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libpatchclampsinglestimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LDFLAGS)

libpatchclampsinglestimulus_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libpatchclampsinglestimulus_la_OBJECTS) : moc_singlestimulus.cc

libpatchclampsinglestimulus_la_SOURCES = singlestimulus.cc

libpatchclampsinglestimulus_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampsinglestimulus_la_include_HEADERS = $(HEADER_PATH)/singlestimulus.h



libpatchclampmembraneresistance_la_CPPFLAGS = \
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

libpatchclampmembraneresistance_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libpatchclampmembraneresistance_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LIBS)

$(libpatchclampmembraneresistance_la_OBJECTS) : moc_membraneresistance.cc

libpatchclampmembraneresistance_la_SOURCES = membraneresistance.cc

libpatchclampmembraneresistance_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampmembraneresistance_la_include_HEADERS = $(HEADER_PATH)/membraneresistance.h



libpatchclampsetdc_la_CPPFLAGS = \
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

libpatchclampsetdc_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libpatchclampsetdc_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT3_LIBS)

$(libpatchclampsetdc_la_OBJECTS) : moc_setdc.cc

libpatchclampsetdc_la_SOURCES = setdc.cc

libpatchclampsetdc_la_includedir = $(pkgincludedir)/patchclamp

libpatchclampsetdc_la_include_HEADERS = $(HEADER_PATH)/setdc.h



check_PROGRAMS = \
    linktest_libpatchclampsession_la \
    linktest_libpatchclampsimple_la \
    linktest_libpatchclampsinglestimulus_la \
    linktest_libpatchclampmembraneresistance_la \
    linktest_libpatchclampsetdc_la

linktest_libpatchclampsession_la_SOURCES = linktest.cc
linktest_libpatchclampsession_la_LDADD = libpatchclampsession.la

linktest_libpatchclampsimple_la_SOURCES = linktest.cc
linktest_libpatchclampsimple_la_LDADD = libpatchclampsimple.la

linktest_libpatchclampsinglestimulus_la_SOURCES = linktest.cc
linktest_libpatchclampsinglestimulus_la_LDADD = libpatchclampsinglestimulus.la

linktest_libpatchclampmembraneresistance_la_SOURCES = linktest.cc
linktest_libpatchclampmembraneresistance_la_LDADD = libpatchclampmembraneresistance.la

linktest_libpatchclampsetdc_la_SOURCES = linktest.cc
linktest_libpatchclampsetdc_la_LDADD = libpatchclampsetdc.la

TESTS = $(check_PROGRAMS)

