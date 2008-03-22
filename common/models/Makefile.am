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

librelacscommonneuronmodels_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacscommonneuronmodels_la-%-moc.lo : librelacscommonneuronmodels_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonneuronmodels_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonneuronmodels_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonneuronmodels_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonneuronmodels_la-neuronmodels-moc.lo

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

librelacscommonsimplemodel_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacscommonsimplemodel_la-%-moc.lo : librelacscommonsimplemodel_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacscommonsimplemodel_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacscommonsimplemodel_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacscommonsimplemodel_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacscommonsimplemodel_la-simplemodel-moc.lo

librelacscommonsimplemodel_la_SOURCES = \
    simplemodel.cc \
    ../include/relacs/simplemodel.h



if RELACS_LINKTESTS
linktest_librelacscommonneuronmodels_la_SOURCES =
linktest_librelacscommonneuronmodels_la_LDADD = librelacscommonneuronmodels.la ../../relacs/src/librelacslinktest.la

linktest_librelacscommonsimplemodel_la_SOURCES =
linktest_librelacscommonsimplemodel_la_LDADD = librelacscommonsimplemodel.la ../../relacs/src/librelacslinktest.la
endif

