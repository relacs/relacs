HEADER_PATH = ../include/relacs/efield
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libefieldlinearfield.la \
    libefieldeoddetector.la \
    libefieldchirpdetector.la \
    libefieldbeatdetector.la



libefieldlinearfield_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldlinearfield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldlinearfield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libefieldlinearfield_la_OBJECTS) : moc_linearfield.cc

libefieldlinearfield_la_SOURCES = linearfield.cc

libefieldlinearfield_la_includedir = $(pkgincludedir)/efield

libefieldlinearfield_la_include_HEADERS = $(HEADER_PATH)/linearfield.h




libefieldeoddetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldeoddetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldeoddetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libefieldeoddetector_la_OBJECTS) : moc_eoddetector.cc

libefieldeoddetector_la_SOURCES = eoddetector.cc

libefieldeoddetector_la_includedir = $(pkgincludedir)/efield

libefieldeoddetector_la_include_HEADERS = $(HEADER_PATH)/eoddetector.h



libefieldchirpdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldchirpdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldchirpdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libefieldchirpdetector_la_OBJECTS) : moc_chirpdetector.cc

libefieldchirpdetector_la_SOURCES = chirpdetector.cc

libefieldchirpdetector_la_includedir = $(pkgincludedir)/efield

libefieldchirpdetector_la_include_HEADERS = $(HEADER_PATH)/chirpdetector.h



libefieldbeatdetector_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libefieldbeatdetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libefieldbeatdetector_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libefieldbeatdetector_la_OBJECTS) : moc_beatdetector.cc

libefieldbeatdetector_la_SOURCES = beatdetector.cc

libefieldbeatdetector_la_includedir = $(pkgincludedir)/efield

libefieldbeatdetector_la_include_HEADERS = $(HEADER_PATH)/beatdetector.h



check_PROGRAMS = \
    linktest_libefieldlinearfield_la \
    linktest_libefieldeoddetector_la \
    linktest_libefieldchirpdetector_la \
    linktest_libefieldbeatdetector_la

linktest_libefieldlinearfield_la_SOURCES = linktest.cc
linktest_libefieldlinearfield_la_LDADD = libefieldlinearfield.la

linktest_libefieldeoddetector_la_SOURCES = linktest.cc
linktest_libefieldeoddetector_la_LDADD = libefieldeoddetector.la

linktest_libefieldchirpdetector_la_SOURCES = linktest.cc
linktest_libefieldchirpdetector_la_LDADD = libefieldchirpdetector.la

linktest_libefieldbeatdetector_la_SOURCES = linktest.cc
linktest_libefieldbeatdetector_la_LDADD = libefieldbeatdetector.la

TESTS = $(check_PROGRAMS)
