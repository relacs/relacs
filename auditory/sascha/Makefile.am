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
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryisoresponse_la-%-moc.cpp : ../include/relacs/%.h
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
    ../include/relacs/isoresponse.h



librelacsauditoryoneclick_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryoneclick_la-%-moc.cpp : ../include/relacs/%.h
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
    ../include/relacs/oneclick.h



if RELACS_LINKTESTS
linktest_librelacsauditoryisoresponse_la_SOURCES =
linktest_librelacsauditoryisoresponse_la_LDADD = librelacsauditoryisoresponse.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryoneclick_la_SOURCES =
linktest_librelacsauditoryoneclick_la_LDADD = librelacsauditoryoneclick.la ../../relacs/src/librelacslinktest.la
endif

