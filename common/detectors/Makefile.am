MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonspikedetector.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonspikedetector_la
endif



librelacscommonspikedetector_la_CPPFLAGS = \
    -I../../config/src \
    -I../../daq/src \
    -I../../datafile/src \
    -I../../mplot/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs/ \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacscommonspikedetector_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacscommonspikedetector_la-%-moc.lo : librelacscommonspikedetector_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonspikedetector_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonspikedetector_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonspikedetector_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonspikedetector_la-spikedetector-moc.lo

librelacscommonspikedetector_la_SOURCES = \
    spikedetector.cc \
    spikedetector.h



if RELACS_LINKTESTS
linktest_librelacscommonspikedetector_la_SOURCES =
linktest_librelacscommonspikedetector_la_LDADD = librelacscommonspikedetector.la ../../relacs/src/librelacslinktest.la
endif

