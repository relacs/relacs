MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonneuronmodels.la \
    librelacscommonsimplemodel.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonneuronmodels_la \
    linktest_librelacscommonsimplemodel_la
endif



librelacscommonneuronmodels_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonneuronmodels_la_LIBADD = \
    ../../relacs/src/librelacs.la

$(librelacscommonneuronmodels_la_OBJECTS) : \
    moc_neuronmodels.cc

librelacscommonneuronmodels_la_SOURCES = \
    neuronmodels.cc \
    ../include/relacs/neuronmodels.h



librelacscommonsimplemodel_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonsimplemodel_la_LIBADD = \
    ../../relacs/src/librelacs.la

$(librelacscommonsimplemodel_la_OBJECTS) : \
    moc_simplemodel.cc

librelacscommonsimplemodel_la_SOURCES = \
    simplemodel.cc \
    ../include/relacs/simplemodel.h



if RELACS_LINKTESTS
linktest_librelacscommonneuronmodels_la_SOURCES =
linktest_librelacscommonneuronmodels_la_LDADD = librelacscommonneuronmodels.la ../../linktest/librelacslinktest.la

linktest_librelacscommonsimplemodel_la_SOURCES =
linktest_librelacscommonsimplemodel_la_LDADD = librelacscommonsimplemodel.la ../../linktest/librelacslinktest.la
endif

