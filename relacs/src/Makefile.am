include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacs.la


librelacs_la_CPPFLAGS = \
    "-DRELACSVERSION=\"$(RELACSVERSION)\"" \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

librelacs_la_LDFLAGS = \
    -version-number 0:0:0 \
    $(QT3_LDFLAGS)

librelacs_la_LIBADD = \
    ../../plot/src/librelacsplot.la \
    ../../widgets/src/librelacswidgets.la \
    ../../datafile/src/librelacsdatafile.la \
    ../../options/src/librelacsoptions.la \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    -ldl \
    $(QT3_LIBS)

$(librelacs_la_OBJECTS) : \
    moc_control.cc \
    moc_defaultrepro.cc \
    moc_defaultsession.cc \
    moc_filter.cc \
    moc_filterdetectors.cc \
    moc_macros.cc \
    moc_metadata.cc \
    moc_plottrace.cc \
    moc_relacsplugin.cc \
    moc_relacswidget.cc \
    moc_repro.cc \
    moc_repros.cc \
    moc_savefiles.cc \
    moc_session.cc \
    moc_settings.cc \
    moc_spiketrace.cc

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
    spiketrace.cc


check_PROGRAMS = linktest_librelacs_la
linktest_librelacs_la_SOURCES = linktest.cc
linktest_librelacs_la_LDADD = librelacs.la
TESTS = $(check_PROGRAMS)

