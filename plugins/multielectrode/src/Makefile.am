HEADER_PATH = ../include/relacs/multielectrode
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libmultielectrodemultista.la \
    libmultielectrodemultitracesta.la


libmultielectrodemultista_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmultielectrodemultista_la_LDFLAGS = \
    -module -avoid-version \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmultielectrodemultista_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT_LIBS) $(NIX_LIBS)

$(libmultielectrodemultista_la_OBJECTS) : moc_multista.cc

libmultielectrodemultista_la_SOURCES = multista.cc

libmultielectrodemultista_la_includedir = $(pkgincludedir)/multielectrode

libmultielectrodemultista_la_include_HEADERS = $(HEADER_PATH)/multista.h


libmultielectrodemultitracesta_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/ephys/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmultielectrodemultitracesta_la_LDFLAGS = \
    -module -avoid-version \
    $(top_builddir)/plugins/ephys/src/libephystraces.la \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmultielectrodemultitracesta_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT_LIBS) $(NIX_LIBS)

$(libmultielectrodemultitracesta_la_OBJECTS) : moc_multitracesta.cc

libmultielectrodemultitracesta_la_SOURCES = multitracesta.cc

libmultielectrodemultitracesta_la_includedir = $(pkgincludedir)/multielectrode

libmultielectrodemultitracesta_la_include_HEADERS = $(HEADER_PATH)/multitracesta.h



check_PROGRAMS = \
    linktest_libmultielectrodemultista_la \
    linktest_libmultielectrodemultitracesta_la

linktest_libmultielectrodemultista_la_SOURCES = linktest.cc
linktest_libmultielectrodemultista_la_LDADD = libmultielectrodemultista.la

linktest_libmultielectrodemultitracesta_la_SOURCES = linktest.cc
linktest_libmultielectrodemultitracesta_la_LDADD = libmultielectrodemultitracesta.la

TESTS = $(check_PROGRAMS)

