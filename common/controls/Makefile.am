MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonmotorcontrol.la \
    librelacscommonspectrumanalyzer.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonmotorcontrol_la \
    linktest_librelacscommonspectrumanalyzer_la
endif



librelacscommonmotorcontrol_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../widgets/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacscommonmotorcontrol_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacscommonmotorcontrol_la-%-moc.lo : librelacscommonmotorcontrol_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonmotorcontrol_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonmotorcontrol_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonmotorcontrol_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonmotorcontrol_la-motorcontrol-moc.lo

librelacscommonmotorcontrol_la_SOURCES = \
    motorcontrol.cc \
    ../include/relacs/motorcontrol.h



librelacscommonspectrumanalyzer_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacscommonspectrumanalyzer_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacscommonspectrumanalyzer_la-%-moc.lo : librelacscommonspectrumanalyzer_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonspectrumanalyzer_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonspectrumanalyzer_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonspectrumanalyzer_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonspectrumanalyzer_la-spectrumanalyzer-moc.lo

librelacscommonspectrumanalyzer_la_SOURCES = \
    spectrumanalyzer.cc \
    ../include/relacs/spectrumanalyzer.h



if RELACS_LINKTESTS
linktest_librelacscommonmotorcontrol_la_SOURCES =
linktest_librelacscommonmotorcontrol_la_LDADD = librelacscommonmotorcontrol.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonspectrumanalyzer_la_SOURCES =
linktest_librelacscommonspectrumanalyzer_la_LDADD = librelacscommonspectrumanalyzer.la ../../relacs/src/librelacslinktest.la
endif

