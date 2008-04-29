include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libauditoryloudspeaker.la \
    libauditorycalibspeakers.la


libauditorycalibspeakers_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorycalibspeakers_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorycalibspeakers_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../base/libauditorytraces.la \
    libauditoryloudspeaker.la \
    $(QT3_LIBS)

$(libauditorycalibspeakers_la_OBJECTS) : \
    moc_calibspeakers.cc

libauditorycalibspeakers_la_SOURCES = \
    calibspeakers.cc

libauditorycalibspeakers_la_includedir = $(pkgincludedir)/auditory

libauditorycalibspeakers_la_include_HEADERS = \
    ../include/relacs/calibspeakers.h



libauditoryloudspeaker_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryloudspeaker_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryloudspeaker_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libauditoryloudspeaker_la_OBJECTS) : \
    moc_loudspeaker.cc

libauditoryloudspeaker_la_SOURCES = \
    loudspeaker.cc

libauditoryloudspeaker_la_includedir = $(pkgincludedir)/auditory

libauditoryloudspeaker_la_include_HEADERS = \
    ../include/relacs/loudspeaker.h


check_PROGRAMS = \
    linktest_libauditorycalibspeakers_la \
    linktest_libauditoryloudspeaker_la

linktest_libauditorycalibspeakers_la_SOURCES = linktest.cc
linktest_libauditorycalibspeakers_la_LDADD = libauditorycalibspeakers.la

linktest_libauditoryloudspeaker_la_SOURCES = linktest.cc
linktest_libauditoryloudspeaker_la_LDADD = libauditoryloudspeaker.la

TESTS = $(check_PROGRAMS)

