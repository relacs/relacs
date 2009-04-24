HEADER_PATH = ../include/relacs/skel
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libskelnewrepro.la



libskelnewrepro_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libskelnewrepro_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libskelnewrepro_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libskelnewrepro_la_OBJECTS) : moc_newrepro.cc

libskelnewrepro_la_SOURCES = newrepro.cc

libskelnewrepro_la_includedir = $(pkgincludedir)/skel

libskelnewrepro_la_include_HEADERS = $(HEADER_PATH)/newrepro.h




check_PROGRAMS = \
    linktest_libskelnewrepro_la

linktest_libskelnewrepro_la_SOURCES = linktest.cc
linktest_libskelnewrepro_la_LDADD = libskelnewrepro.la

TESTS = $(check_PROGRAMS)
