HEADER_PATH = ../include/relacs/common
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonspikedetector.la


libcommonspikedetector_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonspikedetector_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonspikedetector_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonspikedetector_la_OBJECTS) : \
    moc_spikedetector.cc

libcommonspikedetector_la_SOURCES = \
    spikedetector.cc

libcommonspikedetector_la_includedir = $(pkgincludedir)/common

libcommonspikedetector_la_include_HEADERS = \
    $(HEADER_PATH)/spikedetector.h



check_PROGRAMS = linktest_libcommonspikedetector_la
linktest_libcommonspikedetector_la_SOURCES = linktest.cc
linktest_libcommonspikedetector_la_LDADD = libcommonspikedetector.la
TESTS = $(check_PROGRAMS)
