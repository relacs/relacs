HEADER_PATH = ../include/relacs/calibration
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcalibrationrestartdelay.la \
    libcalibrationstimulusdelay.la



libcalibrationrestartdelay_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libcalibrationrestartdelay_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libcalibrationrestartdelay_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcalibrationrestartdelay_la_OBJECTS) : moc_restartdelay.cc

libcalibrationrestartdelay_la_SOURCES = restartdelay.cc

libcalibrationrestartdelay_la_includedir = $(pkgincludedir)/calibration

libcalibrationrestartdelay_la_include_HEADERS = $(HEADER_PATH)/restartdelay.h



libcalibrationstimulusdelay_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libcalibrationstimulusdelay_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libcalibrationstimulusdelay_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcalibrationstimulusdelay_la_OBJECTS) : moc_stimulusdelay.cc

libcalibrationstimulusdelay_la_SOURCES = stimulusdelay.cc

libcalibrationstimulusdelay_la_includedir = $(pkgincludedir)/calibration

libcalibrationstimulusdelay_la_include_HEADERS = $(HEADER_PATH)/stimulusdelay.h



check_PROGRAMS = \
    linktest_libcalibrationrestartdelay_la \
    linktest_libcalibrationstimulusdelay_la

linktest_libcalibrationrestartdelay_la_SOURCES = linktest.cc
linktest_libcalibrationrestartdelay_la_LDADD = libcalibrationrestartdelay.la

linktest_libcalibrationstimulusdelay_la_SOURCES = linktest.cc
linktest_libcalibrationstimulusdelay_la_LDADD = libcalibrationstimulusdelay.la

TESTS = $(check_PROGRAMS)

