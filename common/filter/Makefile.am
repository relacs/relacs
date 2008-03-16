MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonhighpass.la \
    librelacscommonlowpass.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonhighpass_la \
    linktest_librelacscommonlowpass_la
endif



librelacscommonhighpass_la_CPPFLAGS = \
    -I../../config/src \
    -I../../daq/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs/ \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacscommonhighpass_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacscommonhighpass_la-%-moc.lo : librelacscommonhighpass_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonhighpass_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonhighpass_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonhighpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonhighpass_la-highpass-moc.lo

librelacscommonhighpass_la_SOURCES = \
    highpass.cc \
    highpass.h



librelacscommonlowpass_la_CPPFLAGS = \
    -I../../config/src \
    -I../../daq/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs/ \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacscommonlowpass_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacscommonlowpass_la-%-moc.lo : librelacscommonlowpass_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonlowpass_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonlowpass_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonlowpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonlowpass_la-lowpass-moc.lo

librelacscommonlowpass_la_SOURCES = \
    lowpass.cc \
    lowpass.h



if RELACS_LINKTESTS
linktest_librelacscommonhighpass_la_SOURCES =
linktest_librelacscommonhighpass_la_LDADD = librelacscommonhighpass.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonlowpass_la_SOURCES =
linktest_librelacscommonlowpass_la_LDADD = librelacscommonlowpass.la ../../relacs/src/librelacslinktest.la
endif

