MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacsauditoryisoresponse.la \
    librelacsauditoryoneclick.la


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

librelacsauditoryisoresponse_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsauditoryisoresponse_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../base/librelacsauditorysession.la \
    @QT3_LIBS@

$(librelacsauditoryisoresponse_la_OBJECTS) : \
    moc_isoresponse.cc

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

librelacsauditoryoneclick_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsauditoryoneclick_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../base/librelacsauditorytraces.la \
    ../speaker/librelacsauditoryloudspeaker.la \
    @QT3_LIBS@

$(librelacsauditoryoneclick_la_OBJECTS) : \
    moc_oneclick.cc

librelacsauditoryoneclick_la_SOURCES = \
    oneclick.cc \
    ../include/relacs/oneclick.h


check_PROGRAMS = \
    linktest_librelacsauditoryisoresponse_la \
    linktest_librelacsauditoryoneclick_la

linktest_librelacsauditoryisoresponse_la_SOURCES = linktest.cc
linktest_librelacsauditoryisoresponse_la_LDADD = librelacsauditoryisoresponse.la

linktest_librelacsauditoryoneclick_la_SOURCES = linktest.cc
linktest_librelacsauditoryoneclick_la_LDADD = librelacsauditoryoneclick.la

