include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonmotorcontrol.la \
    libcommonspectrumanalyzer.la


libcommonmotorcontrol_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonmotorcontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonmotorcontrol_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonmotorcontrol_la_OBJECTS) : \
    moc_motorcontrol.cc

libcommonmotorcontrol_la_SOURCES = \
    motorcontrol.cc

libcommonmotorcontrol_la_includedir = $(pkgincludedir)/common

libcommonmotorcontrol_la_include_HEADERS = \
    ../include/relacs/motorcontrol.h



libcommonspectrumanalyzer_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonspectrumanalyzer_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonspectrumanalyzer_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonspectrumanalyzer_la_OBJECTS) : \
    moc_spectrumanalyzer.cc

libcommonspectrumanalyzer_la_SOURCES = \
    spectrumanalyzer.cc

libcommonspectrumanalyzer_la_includedir = $(pkgincludedir)/common

libcommonspectrumanalyzer_la_include_HEADERS = \
    ../include/relacs/spectrumanalyzer.h



check_PROGRAMS = \
    linktest_libcommonmotorcontrol_la \
    linktest_libcommonspectrumanalyzer_la

linktest_libcommonmotorcontrol_la_SOURCES = linktest.cc
linktest_libcommonmotorcontrol_la_LDADD = libcommonmotorcontrol.la

linktest_libcommonspectrumanalyzer_la_SOURCES = linktest.cc
linktest_libcommonspectrumanalyzer_la_LDADD = libcommonspectrumanalyzer.la

TESTS = $(check_PROGRAMS)
