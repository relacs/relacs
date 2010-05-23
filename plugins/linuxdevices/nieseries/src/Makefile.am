HEADER_PATH = ../include/relacs/nieseries

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libnieseriesniai.la \
    libnieseriesniao.la \
    libnieseriesnidio.la \
    libnieseriesnipfi.la



libnieseriesniai_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=251 \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libnieseriesniai_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libnieseriesniai_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

libnieseriesniai_la_SOURCES = niai.cc niaidevice.cc

libnieseriesniai_la_includedir = $(pkgincludedir)/nieseries

libnieseriesniai_la_include_HEADERS = \
    $(HEADER_PATH)/nidaq.h \
    $(HEADER_PATH)/niai.h



libnieseriesniao_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=251 \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libnieseriesniao_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libnieseriesniao_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

libnieseriesniao_la_SOURCES = niao.cc niaodevice.cc

libnieseriesniao_la_includedir = $(pkgincludedir)/nieseries

libnieseriesniao_la_include_HEADERS = \
    $(HEADER_PATH)/nidaq.h \
    $(HEADER_PATH)/niao.h



libnieseriesnidio_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=251 \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libnieseriesnidio_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libnieseriesnidio_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

libnieseriesnidio_la_SOURCES = nidio.cc nidiodevice.cc

libnieseriesnidio_la_includedir = $(pkgincludedir)/nieseries

libnieseriesnidio_la_include_HEADERS = \
    $(HEADER_PATH)/nidaq.h \
    $(HEADER_PATH)/nidio.h



libnieseriesnipfi_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=251 \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libnieseriesnipfi_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libnieseriesnipfi_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

libnieseriesnipfi_la_SOURCES = nipfi.cc nipfidevice.cc

libnieseriesnipfi_la_includedir = $(pkgincludedir)/nieseries

libnieseriesnipfi_la_include_HEADERS = \
    $(HEADER_PATH)/nidaq.h \
    $(HEADER_PATH)/nipfi.h




check_PROGRAMS = \
    linktest_libnieseriesniai_la \
    linktest_libnieseriesniao_la \
    linktest_libnieseriesnidio_la \
    linktest_libnieseriesnipfi_la

linktest_libnieseriesniai_la_SOURCES = linktest.cc
linktest_libnieseriesniai_la_LDADD = libnieseriesniai.la

linktest_libnieseriesniao_la_SOURCES = linktest.cc
linktest_libnieseriesniao_la_LDADD = libnieseriesniao.la

linktest_libnieseriesnidio_la_SOURCES = linktest.cc
linktest_libnieseriesnidio_la_LDADD = libnieseriesnidio.la

linktest_libnieseriesnipfi_la_SOURCES = linktest.cc
linktest_libnieseriesnipfi_la_LDADD = libnieseriesnipfi.la

TESTS = $(check_PROGRAMS)
