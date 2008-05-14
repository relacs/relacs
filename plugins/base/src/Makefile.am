HEADER_PATH = ../include/relacs/base
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libbasehighpass.la \
    libbaselowpass.la \
    libbasespectrumanalyzer.la \
    libbasepause.la \
    libbasesimplemodel.la



libbasehighpass_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libbasehighpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libbasehighpass_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libbasehighpass_la_OBJECTS) : moc_highpass.cc

libbasehighpass_la_SOURCES = highpass.cc

libbasehighpass_la_includedir = $(pkgincludedir)/base

libbasehighpass_la_include_HEADERS = $(HEADER_PATH)/highpass.h



libbaselowpass_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libbaselowpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libbaselowpass_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libbaselowpass_la_OBJECTS) : moc_lowpass.cc

libbaselowpass_la_SOURCES = lowpass.cc

libbaselowpass_la_includedir = $(pkgincludedir)/base

libbaselowpass_la_include_HEADERS = $(HEADER_PATH)/lowpass.h



libbasespectrumanalyzer_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libbasespectrumanalyzer_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libbasespectrumanalyzer_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libbasespectrumanalyzer_la_OBJECTS) : moc_spectrumanalyzer.cc

libbasespectrumanalyzer_la_SOURCES = spectrumanalyzer.cc

libbasespectrumanalyzer_la_includedir = $(pkgincludedir)/base

libbasespectrumanalyzer_la_include_HEADERS = $(HEADER_PATH)/spectrumanalyzer.h



libbasepause_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libbasepause_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libbasepause_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libbasepause_la_OBJECTS) : moc_pause.cc

libbasepause_la_SOURCES = pause.cc

libbasepause_la_includedir = $(pkgincludedir)/base

libbasepause_la_include_HEADERS = $(HEADER_PATH)/pause.h



libbasesimplemodel_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libbasesimplemodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libbasesimplemodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libbasesimplemodel_la_OBJECTS) : moc_simplemodel.cc

libbasesimplemodel_la_SOURCES = simplemodel.cc

libbasesimplemodel_la_includedir = $(pkgincludedir)/base

libbasesimplemodel_la_include_HEADERS = $(HEADER_PATH)/simplemodel.h



check_PROGRAMS = \
    linktest_libbasehighpass_la \
    linktest_libbaselowpass_la \
    linktest_libbasespectrumanalyzer_la \
    linktest_libbasepause_la \
    linktest_libbasesimplemodel_la

linktest_libbasehighpass_la_SOURCES = linktest.cc
linktest_libbasehighpass_la_LDADD = libbasehighpass.la

linktest_libbaselowpass_la_SOURCES = linktest.cc
linktest_libbaselowpass_la_LDADD = libbaselowpass.la

linktest_libbasespectrumanalyzer_la_SOURCES = linktest.cc
linktest_libbasespectrumanalyzer_la_LDADD = libbasespectrumanalyzer.la

linktest_libbasepause_la_SOURCES = linktest.cc
linktest_libbasepause_la_LDADD = libbasepause.la

linktest_libbasesimplemodel_la_SOURCES = linktest.cc
linktest_libbasesimplemodel_la_LDADD = libbasesimplemodel.la

TESTS = $(check_PROGRAMS)
