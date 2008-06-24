HEADER_PATH = ../include/relacs/attcs3310

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libattcs3310cs3310pp.la
if RELACS_COND_nieseries
pluginlib_LTLIBRARIES += \
    libattcs3310cs3310nidio.la \
    libattcs3310cs3310serialnidio.la
endif


libattcs3310cs3310pp_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libattcs3310cs3310pp_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libattcs3310cs3310pp_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT3_LIBS)

libattcs3310cs3310pp_la_SOURCES = cs3310pp.cc cs3310ppdevice.cc

libattcs3310cs3310pp_la_includedir = $(pkgincludedir)/attcs3310

libattcs3310cs3310pp_la_include_HEADERS = $(HEADER_PATH)/cs3310pp.h



libattcs3310cs3310nidio_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../../nieseries/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libattcs3310cs3310nidio_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libattcs3310cs3310nidio_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/linuxdevices/nieseries/src/libnieseriesnidio.la \
    $(QT3_LIBS)

libattcs3310cs3310nidio_la_SOURCES = cs3310nidio.cc cs3310nidiodevice.cc

libattcs3310cs3310nidio_la_includedir = $(pkgincludedir)/attcs3310

libattcs3310cs3310nidio_la_include_HEADERS = $(HEADER_PATH)/cs3310nidio.h



libattcs3310cs3310serialnidio_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../../nieseries/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libattcs3310cs3310serialnidio_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libattcs3310cs3310serialnidio_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/linuxdevices/nieseries/src/libnieseriesnidio.la \
    $(QT3_LIBS)

libattcs3310cs3310serialnidio_la_SOURCES = cs3310serialnidio.cc cs3310serialnidiodevice.cc

libattcs3310cs3310serialnidio_la_includedir = $(pkgincludedir)/attcs3310

libattcs3310cs3310serialnidio_la_include_HEADERS = $(HEADER_PATH)/cs3310serialnidio.h



check_PROGRAMS = \
    linktest_libattcs3310cs3310pp_la \
    linktest_libattcs3310cs3310nidio_la \
    linktest_libattcs3310cs3310serialnidio_la

linktest_libattcs3310cs3310pp_la_SOURCES = linktest.cc
linktest_libattcs3310cs3310pp_la_LDADD = libattcs3310cs3310pp.la

linktest_libattcs3310cs3310nidio_la_SOURCES = linktest.cc
linktest_libattcs3310cs3310nidio_la_LDADD = libattcs3310cs3310nidio.la

linktest_libattcs3310cs3310serialnidio_la_SOURCES = linktest.cc
linktest_libattcs3310cs3310serialnidio_la_LDADD = libattcs3310cs3310serialnidio.la

TESTS = $(check_PROGRAMS)
