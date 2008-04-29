include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcommonneuronmodels.la \
    libcommonsimplemodel.la


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
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

$(libcommonneuronmodels_la_OBJECTS) : \
    moc_neuronmodels.cc

libcommonneuronmodels_la_SOURCES = \
    neuronmodels.cc

libcommonneuronmodels_la_includedir = $(pkgincludedir)/common

libcommonneuronmodels_la_include_HEADERS = \
    ../include/relacs/neuronmodels.h



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

$(libcommonsimplemodel_la_OBJECTS) : \
    moc_simplemodel.cc

libcommonsimplemodel_la_SOURCES = \
    simplemodel.cc

libcommonsimplemodel_la_includedir = $(pkgincludedir)/common

libcommonsimplemodel_la_include_HEADERS = \
    ../include/relacs/simplemodel.h


check_PROGRAMS = \
    linktest_libcommonneuronmodels_la \
    linktest_libcommonsimplemodel_la

linktest_libcommonneuronmodels_la_SOURCES = linktest.cc
linktest_libcommonneuronmodels_la_LDADD = libcommonneuronmodels.la

linktest_libcommonsimplemodel_la_SOURCES = linktest.cc
linktest_libcommonsimplemodel_la_LDADD = libcommonsimplemodel.la

TESTS = $(check_PROGRAMS)

