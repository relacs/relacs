include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonmotorcontrol.la \
    librelacscommonspectrumanalyzer.la



librelacscommonmotorcontrol_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonmotorcontrol_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonmotorcontrol_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonmotorcontrol_la_OBJECTS) : \
    moc_motorcontrol.cc

librelacscommonmotorcontrol_la_SOURCES = \
    motorcontrol.cc \
    ../include/relacs/motorcontrol.h



librelacscommonspectrumanalyzer_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonspectrumanalyzer_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonspectrumanalyzer_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonspectrumanalyzer_la_OBJECTS) : \
    moc_spectrumanalyzer.cc

librelacscommonspectrumanalyzer_la_SOURCES = \
    spectrumanalyzer.cc \
    ../include/relacs/spectrumanalyzer.h



check_PROGRAMS = \
    linktest_librelacscommonmotorcontrol_la \
    linktest_librelacscommonspectrumanalyzer_la

linktest_librelacscommonmotorcontrol_la_SOURCES = linktest.cc
linktest_librelacscommonmotorcontrol_la_LDADD = librelacscommonmotorcontrol.la

linktest_librelacscommonspectrumanalyzer_la_SOURCES = linktest.cc
linktest_librelacscommonspectrumanalyzer_la_LDADD = librelacscommonspectrumanalyzer.la

