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
    -I../../daq/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../include

librelacscommonephystraces_la_LIBADD = \
    ../../relacs/src/librelacs.la

librelacscommonephystraces_la_SOURCES = \
    ephystraces.cc \
    ../include/relacs/ephystraces.h



librelacscommonpause_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacscommonpause_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacscommonpause_la-%-moc.lo : librelacscommonpause_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonpause_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonpause_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonpause_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonpause_la-pause-moc.lo

librelacscommonpause_la_SOURCES = \
    pause.cc \
    ../include/relacs/pause.h



librelacscommonrestartdelay_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacscommonrestartdelay_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacscommonrestartdelay_la-%-moc.lo : librelacscommonrestartdelay_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonrestartdelay_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonrestartdelay_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonrestartdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonrestartdelay_la-restartdelay-moc.lo

librelacscommonrestartdelay_la_SOURCES = \
    restartdelay.cc \
    ../include/relacs/restartdelay.h



librelacscommonstimulusdelay_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacscommonstimulusdelay_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacscommonstimulusdelay_la-%-moc.lo : librelacscommonstimulusdelay_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonstimulusdelay_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonstimulusdelay_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonstimulusdelay_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonstimulusdelay_la-stimulusdelay-moc.lo

librelacscommonstimulusdelay_la_SOURCES = \
    stimulusdelay.cc \
    ../include/relacs/stimulusdelay.h



if RELACS_LINKTESTS
linktest_librelacscommonephystraces_la_SOURCES =
linktest_librelacscommonephystraces_la_LDADD = librelacscommonephystraces.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonpause_la_SOURCES =
linktest_librelacscommonpause_la_LDADD = librelacscommonpause.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonrestartdelay_la_SOURCES =
linktest_librelacscommonrestartdelay_la_LDADD = librelacscommonrestartdelay.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonstimulusdelay_la_SOURCES =
linktest_librelacscommonstimulusdelay_la_LDADD = librelacscommonstimulusdelay.la ../../relacs/src/librelacslinktest.la
endif

