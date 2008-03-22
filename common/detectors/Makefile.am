MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacscommonspikedetector.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacscommonspikedetector_la
endif



librelacscommonspikedetector_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacscommonspikedetector_la_LIBADD = \
    ../../relacs/src/librelacs.la

librelacscommonspikedetector.la : \
    moc_spikedetector.cc

librelacscommonspikedetector_la_SOURCES = \
    spikedetector.cc \
    ../include/relacs/spikedetector.h



if RELACS_LINKTESTS
linktest_librelacscommonspikedetector_la_SOURCES =
linktest_librelacscommonspikedetector_la_LDADD = librelacscommonspikedetector.la ../../relacs/src/librelacslinktest.la
endif

