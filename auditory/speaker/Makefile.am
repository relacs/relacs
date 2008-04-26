include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/$(PLUGINS_BASENAME)

lib_LTLIBRARIES = \
    librelacsauditoryloudspeaker.la \
    librelacsauditorycalibspeakers.la


librelacsauditorycalibspeakers_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

librelacsauditorycalibspeakers_la_LDFLAGS = \
    $(QT3_LDFLAGS)

librelacsauditorycalibspeakers_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../config/src/librelacsconfig.la \
    ../base/librelacsauditorytraces.la \
    librelacsauditoryloudspeaker.la \
    $(QT3_LIBS)

$(librelacsauditorycalibspeakers_la_OBJECTS) : \
    moc_calibspeakers.cc

librelacsauditorycalibspeakers_la_SOURCES = \
    calibspeakers.cc \
    ../include/relacs/calibspeakers.h



librelacsauditoryloudspeaker_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

librelacsauditoryloudspeaker_la_LDFLAGS = \
    $(QT3_LDFLAGS)

librelacsauditoryloudspeaker_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(librelacsauditoryloudspeaker_la_OBJECTS) : \
    moc_loudspeaker.cc

librelacsauditoryloudspeaker_la_SOURCES = \
    loudspeaker.cc \
    ../include/relacs/loudspeaker.h


check_PROGRAMS = \
    linktest_librelacsauditorycalibspeakers_la \
    linktest_librelacsauditoryloudspeaker_la

linktest_librelacsauditorycalibspeakers_la_SOURCES = linktest.cc
linktest_librelacsauditorycalibspeakers_la_LDADD = librelacsauditorycalibspeakers.la

linktest_librelacsauditoryloudspeaker_la_SOURCES = linktest.cc
linktest_librelacsauditoryloudspeaker_la_LDADD = librelacsauditoryloudspeaker.la

TESTS = $(check_PROGRAMS)

