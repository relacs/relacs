HEADER_PATH = ../include/relacs/efield
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libefieldlinearfield.la



libefieldlinearfield_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libefieldlinearfield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libefieldlinearfield_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libefieldlinearfield_la_OBJECTS) : moc_linearfield.cc

libefieldlinearfield_la_SOURCES = linearfield.cc

libefieldlinearfield_la_includedir = $(pkgincludedir)/efield

libefieldlinearfield_la_include_HEADERS = $(HEADER_PATH)/linearfield.h




check_PROGRAMS = \
    linktest_libefieldlinearfield_la

linktest_libefieldlinearfield_la_SOURCES = linktest.cc
linktest_libefieldlinearfield_la_LDADD = libefieldlinearfield.la

TESTS = $(check_PROGRAMS)
