MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacs.la

if RELACS_LINKTESTS
noinst_libdir = $(abs_builddir)

noinst_lib_LTLIBRARIES = \
    librelacslinktest.la
endif



librelacs_la_CPPFLAGS = \
    "-DRELACSVERSION=\"@RELACSVERSION@\"" \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacs_la_LDFLAGS = @QT3_LDFLAGS@

librelacs_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacs_la-%-moc.lo : librelacs_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacs_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacs_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacs_la_LIBADD = \
    ../../config/src/librelacsconfig.la \
    ../../daq/src/librelacsdaq.la \
    ../../datafile/src/librelacsdatafile.la \
    ../../mplot/src/librelacsmplot.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../../widgets/src/librelacswidgets.la \
    \
    librelacs_la-control-moc.lo \
    librelacs_la-defaultrepro-moc.lo \
    librelacs_la-defaultsession-moc.lo \
    librelacs_la-filter-moc.lo \
    librelacs_la-filterdetectors-moc.lo \
    librelacs_la-macros-moc.lo \
    librelacs_la-metadata-moc.lo \
    librelacs_la-plottrace-moc.lo \
    librelacs_la-relacsplugin-moc.lo \
    librelacs_la-relacswidget-moc.lo \
    librelacs_la-repro-moc.lo \
    librelacs_la-repros-moc.lo \
    librelacs_la-savefiles-moc.lo \
    librelacs_la-session-moc.lo \
    librelacs_la-settings-moc.lo \
    librelacs_la-spiketrace-moc.lo

librelacs_ladir = $(includedir)/relacs

librelacs_la_HEADERS = \
    ../include/relacs/aisim.h \
    ../include/relacs/aosim.h \
    ../include/relacs/attsim.h \
    ../include/relacs/control.h \
    ../include/relacs/defaultrepro.h \
    ../include/relacs/defaultsession.h \
    ../include/relacs/filterdetectors.h \
    ../include/relacs/filter.h \
    ../include/relacs/macros.h \
    ../include/relacs/metadata.h \
    ../include/relacs/model.h \
    ../include/relacs/plottrace.h \
    ../include/relacs/plugins.h \
    ../include/relacs/rangeloop.h \
    ../include/relacs/relacsplugin.h \
    ../include/relacs/relacswidget.h \
    ../include/relacs/repro.h \
    ../include/relacs/repros.h \
    ../include/relacs/savefiles.h \
    ../include/relacs/session.h \
    ../include/relacs/settings.h \
    ../include/relacs/simulator.h \
    ../include/relacs/spiketrace.h \
    \
    ../include/relacs/devicelist.h \
    ../include/relacs/relacsdevices.h

librelacs_la_SOURCES = \
    aisim.cc \
    aosim.cc \
    attsim.cc \
    control.cc \
    defaultrepro.cc \
    defaultsession.cc \
    filter.cc \
    filterdetectors.cc \
    macros.cc \
    metadata.cc \
    model.cc \
    plottrace.cc \
    plugins.cc \
    rangeloop.cc \
    relacsplugin.cc \
    relacswidget.cc \
    repro.cc \
    repros.cc \
    savefiles.cc \
    session.cc \
    settings.cc \
    simulator.cc \
    spiketrace.cc \
    \
    relacsdevices.cc



if RELACS_LINKTESTS
librelacslinktest_la_SOURCES = \
    linktest.cc
endif

