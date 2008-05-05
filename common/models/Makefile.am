HEADER_PATH = ../include/relacs/common
include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonsimplemodel.la \
    libcommonspikingneuron.la \
    libcommonneuronmodels.la


libcommonsimplemodel_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonsimplemodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonsimplemodel_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonsimplemodel_la_OBJECTS) : moc_simplemodel.cc

libcommonsimplemodel_la_SOURCES = simplemodel.cc

libcommonsimplemodel_la_includedir = $(pkgincludedir)/common

libcommonsimplemodel_la_include_HEADERS = $(HEADER_PATH)/simplemodel.h


libcommonspikingneuron_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

libcommonspikingneuron_la_LDFLAGS = -version-info=0:0:0

libcommonspikingneuron_la_LIBADD = ../../options/src/librelacsoptions.la

libcommonspikingneuron_la_SOURCES = spikingneuron.cc

libcommonspikingneuron_la_includedir = $(pkgincludedir)/common

libcommonspikingneuron_la_include_HEADERS = $(HEADER_PATH)/spikingneuron.h


libcommonneuronmodels_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libcommonneuronmodels_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libcommonneuronmodels_la_LIBADD = \
    libcommonspikingneuron.la \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonneuronmodels_la_OBJECTS) : moc_neuronmodels.cc

libcommonneuronmodels_la_SOURCES = neuronmodels.cc

libcommonneuronmodels_la_includedir = $(pkgincludedir)/common

libcommonneuronmodels_la_include_HEADERS = $(HEADER_PATH)/neuronmodels.h


check_PROGRAMS = \
    linktest_libcommonsimplemodel_la \
    linktest_libcommonspikingneuron_la \
    linktest_libcommonneuronmodels_la

linktest_libcommonsimplemodel_la_SOURCES = linktest.cc
linktest_libcommonsimplemodel_la_LDADD = libcommonsimplemodel.la

linktest_libcommonspikingneuron_la_SOURCES = linktest.cc
linktest_libcommonspikingneuron_la_LDADD = libcommonspikingneuron.la

linktest_libcommonneuronmodels_la_SOURCES = linktest.cc
linktest_libcommonneuronmodels_la_LDADD = libcommonneuronmodels.la

TESTS = $(check_PROGRAMS)

