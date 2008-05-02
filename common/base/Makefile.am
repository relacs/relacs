HEADER_PATH = ../include/relacs/common
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonephystraces.la \
    libcommonpause.la \
    libcommonrestartdelay.la \
    libcommonstimulusdelay.la


libcommonephystraces_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

libcommonephystraces_la_LDFLAGS = \
    -module -avoid-version

libcommonephystraces_la_LIBADD = \
    ../../relacs/src/librelacs.la

libcommonephystraces_la_SOURCES = \
    ephystraces.cc

libcommonephystraces_la_includedir = $(pkgincludedir)/common

libcommonephystraces_la_include_HEADERS = \
    $(HEADER_PATH)/ephystraces.h



libcommonpause_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonpause_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonpause_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonpause_la_OBJECTS) : \
    moc_pause.cc

libcommonpause_la_SOURCES = \
    pause.cc

libcommonpause_la_includedir = $(pkgincludedir)/common

libcommonpause_la_include_HEADERS = \
    $(HEADER_PATH)/pause.h



libcommonrestartdelay_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonrestartdelay_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonrestartdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonrestartdelay_la_OBJECTS) : \
    moc_restartdelay.cc

libcommonrestartdelay_la_SOURCES = \
    restartdelay.cc

libcommonrestartdelay_la_includedir = $(pkgincludedir)/common

libcommonrestartdelay_la_include_HEADERS = \
    $(HEADER_PATH)/restartdelay.h



libcommonstimulusdelay_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonstimulusdelay_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonstimulusdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonstimulusdelay_la_OBJECTS) : \
    moc_stimulusdelay.cc

libcommonstimulusdelay_la_SOURCES = \
    stimulusdelay.cc

libcommonstimulusdelay_la_includedir = $(pkgincludedir)/common

libcommonstimulusdelay_la_include_HEADERS = \
    $(HEADER_PATH)/stimulusdelay.h



check_PROGRAMS = \
    linktest_libcommonephystraces_la \
    linktest_libcommonpause_la \
    linktest_libcommonrestartdelay_la \
    linktest_libcommonstimulusdelay_la

linktest_libcommonephystraces_la_SOURCES = linktest.cc
linktest_libcommonephystraces_la_LDADD = libcommonephystraces.la

linktest_libcommonpause_la_SOURCES = linktest.cc
linktest_libcommonpause_la_LDADD = libcommonpause.la

linktest_libcommonrestartdelay_la_SOURCES = linktest.cc
linktest_libcommonrestartdelay_la_LDADD = libcommonrestartdelay.la

linktest_libcommonstimulusdelay_la_SOURCES = linktest.cc
linktest_libcommonstimulusdelay_la_LDADD = libcommonstimulusdelay.la

TESTS = $(check_PROGRAMS)

