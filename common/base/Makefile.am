MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonephystraces.la \
    librelacscommonpause.la \
    librelacscommonrestartdelay.la \
    librelacscommonstimulusdelay.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonephystraces_la \
    linktest_librelacscommonpause_la \
    linktest_librelacscommonrestartdelay_la \
    linktest_librelacscommonstimulusdelay_la
endif


librelacscommonephystraces_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

librelacscommonephystraces_la_LIBADD = \
    ../../relacs/src/librelacs.la

librelacscommonephystraces_la_SOURCES = \
    ephystraces.cc \
    ../include/relacs/ephystraces.h



librelacscommonpause_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonpause_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonpause_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonpause_la_OBJECTS) : \
    moc_pause.cc

librelacscommonpause_la_SOURCES = \
    pause.cc \
    ../include/relacs/pause.h



librelacscommonrestartdelay_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonrestartdelay_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonrestartdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonrestartdelay_la_OBJECTS) : \
    moc_restartdelay.cc

librelacscommonrestartdelay_la_SOURCES = \
    restartdelay.cc \
    ../include/relacs/restartdelay.h



librelacscommonstimulusdelay_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonstimulusdelay_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonstimulusdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonstimulusdelay_la_OBJECTS) : \
    moc_stimulusdelay.cc

librelacscommonstimulusdelay_la_SOURCES = \
    stimulusdelay.cc \
    ../include/relacs/stimulusdelay.h



if RELACS_LINKTESTS
linktest_librelacscommonephystraces_la_SOURCES =
linktest_librelacscommonephystraces_la_LDADD = librelacscommonephystraces.la ../../linktest/librelacslinktest.la

linktest_librelacscommonpause_la_SOURCES =
linktest_librelacscommonpause_la_LDADD = librelacscommonpause.la ../../linktest/librelacslinktest.la

linktest_librelacscommonrestartdelay_la_SOURCES =
linktest_librelacscommonrestartdelay_la_LDADD = librelacscommonrestartdelay.la ../../linktest/librelacslinktest.la

linktest_librelacscommonstimulusdelay_la_SOURCES =
linktest_librelacscommonstimulusdelay_la_LDADD = librelacscommonstimulusdelay.la ../../linktest/librelacslinktest.la
endif

