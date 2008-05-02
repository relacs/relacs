HEADER_PATH = ../include/relacs/auditory
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libauditoryisoresponse.la \
    libauditoryoneclick.la


libauditoryisoresponse_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryisoresponse_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryisoresponse_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    ../base/libauditorysession.la \
    $(QT3_LIBS)

$(libauditoryisoresponse_la_OBJECTS) : \
    moc_isoresponse.cc

libauditoryisoresponse_la_SOURCES = \
    isoresponse.cc

libauditoryisoresponse_la_includedir = $(pkgincludedir)/auditory

libauditoryisoresponse_la_include_HEADERS = \
    $(HEADER_PATH)/isoresponse.h



libauditoryoneclick_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryoneclick_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryoneclick_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    ../base/libauditorytraces.la \
    ../speaker/libauditoryloudspeaker.la \
    $(QT3_LIBS)

$(libauditoryoneclick_la_OBJECTS) : \
    moc_oneclick.cc

libauditoryoneclick_la_SOURCES = \
    oneclick.cc

libauditoryoneclick_la_includedir = $(pkgincludedir)/auditory

libauditoryoneclick_la_include_HEADERS = \
    $(HEADER_PATH)/oneclick.h


check_PROGRAMS = \
    linktest_libauditoryisoresponse_la \
    linktest_libauditoryoneclick_la

linktest_libauditoryisoresponse_la_SOURCES = linktest.cc
linktest_libauditoryisoresponse_la_LDADD = libauditoryisoresponse.la

linktest_libauditoryoneclick_la_SOURCES = linktest.cc
linktest_libauditoryoneclick_la_LDADD = libauditoryoneclick.la

TESTS = $(check_PROGRAMS)

