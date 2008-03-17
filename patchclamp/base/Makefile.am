MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacspatchsimple.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacspatchsimple_la
endif



librelacspatchsimple_la_CPPFLAGS = \
    -I../../config/src \
    -I../../daq/src \
    -I../../mplot/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs \
    @QT3_CPPFLAGS@

librelacspatchsimple_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacspatchsimple_la-%-moc.lo : librelacspatchsimple_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacspatchsimple_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacspatchsimple_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacspatchsimple_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacspatchsimple_la-simple-moc.lo

librelacspatchsimple_la_SOURCES = \
    simple.cc \
    simple.h



if RELACS_LINKTESTS
linktest_librelacspatchsimple_la_SOURCES =
linktest_librelacspatchsimple_la_LDADD = librelacspatchsimple.la ../../relacs/src/librelacslinktest.la
endif

