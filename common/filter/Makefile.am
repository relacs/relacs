MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonhighpass.la \
    librelacscommonlowpass.la

EXTRA_PROGRAMS = \
    linktest_librelacscommonhighpass_la \
    linktest_librelacscommonlowpass_la


librelacscommonhighpass_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonhighpass_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonhighpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonhighpass_la_OBJECTS) : \
    moc_highpass.cc

librelacscommonhighpass_la_SOURCES = \
    highpass.cc \
    ../include/relacs/highpass.h



librelacscommonlowpass_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonlowpass_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacscommonlowpass_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    @QT3_LIBS@

$(librelacscommonlowpass_la_OBJECTS) : \
    moc_lowpass.cc

librelacscommonlowpass_la_SOURCES = \
    lowpass.cc \
    ../include/relacs/lowpass.h



if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonhighpass_la \
    linktest_librelacscommonlowpass_la

linktest_librelacscommonhighpass_la_SOURCES = linktest.cc
linktest_librelacscommonhighpass_la_LDADD = librelacscommonhighpass.la

linktest_librelacscommonlowpass_la_SOURCES = linktest.cc
linktest_librelacscommonlowpass_la_LDADD = librelacscommonlowpass.la
endif

