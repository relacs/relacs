MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacsauditoryloudspeaker.la \
    librelacsauditorycalibspeakers.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacsauditorycalibspeakers_la \
    linktest_librelacsauditoryloudspeaker_la
endif



librelacsauditorycalibspeakers_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacsauditorycalibspeakers_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditorycalibspeakers_la-%-moc.lo : librelacsauditorycalibspeakers_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditorycalibspeakers_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditorycalibspeakers_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditorycalibspeakers_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../config/src/librelacsconfig.la \
    ../base/librelacsauditorytraces.la \
    librelacsauditoryloudspeaker.la \
    librelacsauditorycalibspeakers_la-calibspeakers-moc.lo

librelacsauditorycalibspeakers_la_SOURCES = \
    calibspeakers.cc \
    ../include/relacs/calibspeakers.h



librelacsauditoryloudspeaker_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacsauditoryloudspeaker_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditoryloudspeaker_la-%-moc.lo : librelacsauditoryloudspeaker_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryloudspeaker_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryloudspeaker_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryloudspeaker_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacsauditoryloudspeaker_la-loudspeaker-moc.lo

librelacsauditoryloudspeaker_la_SOURCES = \
    loudspeaker.cc \
    ../include/relacs/loudspeaker.h



if RELACS_LINKTESTS
linktest_librelacsauditorycalibspeakers_la_SOURCES =
linktest_librelacsauditorycalibspeakers_la_LDADD = librelacsauditorycalibspeakers.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryloudspeaker_la_SOURCES =
linktest_librelacsauditoryloudspeaker_la_LDADD = librelacsauditoryloudspeaker.la ../../relacs/src/librelacslinktest.la
endif

