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
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include \
    -I../../widgets/include \
    -I../include \
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
    ../include/relacs/spikedetector.h



if RELACS_LINKTESTS
linktest_librelacscommonspikedetector_la_SOURCES =
linktest_librelacscommonspikedetector_la_LDADD = librelacscommonspikedetector.la ../../relacs/src/librelacslinktest.la
endif

