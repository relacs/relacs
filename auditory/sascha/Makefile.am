MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacsauditoryisoresponse.la \
    librelacsauditoryoneclick.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacsauditoryisoresponse_la \
    linktest_librelacsauditoryoneclick_la
endif



librelacsauditoryisoresponse_la_CPPFLAGS = \
    -I../base \
    -I../../common/base \
    -I../../config/src \
    -I../../daq/src \
    -I../../datafile/src \
    -I../../hardware \
    -I../../mplot/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs/ \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacsauditoryisoresponse_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacsauditoryisoresponse_la-%-moc.lo : librelacsauditoryisoresponse_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryisoresponse_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryisoresponse_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryisoresponse_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../base/librelacsauditorysession.la \
    librelacsauditoryisoresponse_la-isoresponse-moc.lo

librelacsauditoryisoresponse_la_SOURCES = \
    isoresponse.cc \
    isoresponse.h



librelacsauditoryoneclick_la_CPPFLAGS = \
    -I../base \
    -I../../common/base \
    -I../../config/src \
    -I../../daq/src \
    -I../../datafile/src \
    -I../../hardware \
    -I../../mplot/src \
    -I../../numerics/src \
    -I../../options/src \
    -I../../relacs/include/relacs/ \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacsauditoryoneclick_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacsauditoryoneclick_la-%-moc.lo : librelacsauditoryoneclick_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryoneclick_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryoneclick_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryoneclick_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../base/librelacsauditorytraces.la \
    ../speaker/librelacsauditoryloudspeaker.la \
    librelacsauditoryoneclick_la-oneclick-moc.lo

librelacsauditoryoneclick_la_SOURCES = \
    oneclick.cc \
    oneclick.h



if RELACS_LINKTESTS
linktest_librelacsauditoryisoresponse_la_SOURCES =
linktest_librelacsauditoryisoresponse_la_LDADD = librelacsauditoryisoresponse.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryoneclick_la_SOURCES =
linktest_librelacsauditoryoneclick_la_LDADD = librelacsauditoryoneclick.la ../../relacs/src/librelacslinktest.la
endif

