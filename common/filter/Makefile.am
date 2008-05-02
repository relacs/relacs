HEADER_PATH = ../include/relacs/common
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonhighpass.la \
    libcommonlowpass.la


libcommonhighpass_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonhighpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonhighpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonhighpass_la_OBJECTS) : \
    moc_highpass.cc

libcommonhighpass_la_SOURCES = \
    highpass.cc

libcommonhighpass_la_includedir = $(pkgincludedir)/common

libcommonhighpass_la_include_HEADERS = \
    $(HEADER_PATH)/highpass.h



libcommonlowpass_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonlowpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonlowpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonlowpass_la_OBJECTS) : \
    moc_lowpass.cc

libcommonlowpass_la_SOURCES = \
    lowpass.cc

libcommonlowpass_la_includedir = $(pkgincludedir)/common

libcommonlowpass_la_include_HEADERS = \
    $(HEADER_PATH)/lowpass.h



check_PROGRAMS = \
    linktest_libcommonhighpass_la \
    linktest_libcommonlowpass_la

linktest_libcommonhighpass_la_SOURCES = linktest.cc
linktest_libcommonhighpass_la_LDADD = libcommonhighpass.la

linktest_libcommonlowpass_la_SOURCES = linktest.cc
linktest_libcommonlowpass_la_LDADD = libcommonlowpass.la

TESTS = $(check_PROGRAMS)

