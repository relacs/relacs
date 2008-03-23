MOC = @MOC@
include ${top_srcdir}/moc3.mk

libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacsauditorytraces.la \
    librelacsauditorysession.la \
    librelacsauditoryficurve.la \
    librelacsauditoryneuron.la \
    librelacsauditorysearch.la \
    librelacsauditoryfifield.la \
    librelacsauditorysinglestimulus.la \
    librelacsauditoryspikeprecision.la \
    librelacsauditorysyslatency.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacsauditoryneuron_la \
    linktest_librelacsauditorysearch_la \
    linktest_librelacsauditorysession_la \
    linktest_librelacsauditorytraces_la \
    linktest_librelacsauditoryficurve_la \
    linktest_librelacsauditoryfifield_la \
    linktest_librelacsauditorysinglestimulus_la \
    linktest_librelacsauditoryspikeprecision_la \
    linktest_librelacsauditorysyslatency_la
endif



librelacsauditoryneuron_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryneuron_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/models/librelacscommonneuronmodels.la

$(librelacsauditoryneuron_la_OBJECTS) : \
    moc_auditoryneuron.cc

librelacsauditoryneuron_la_SOURCES = \
    auditoryneuron.cc \
    ../include/relacs/auditoryneuron.h



librelacsauditorysearch_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditorysearch_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacsauditorytraces.la

$(librelacsauditorysearch_la_OBJECTS) : \
    moc_auditorysearch.cc

librelacsauditorysearch_la_SOURCES = \
    auditorysearch.cc \
    ../include/relacs/auditorysearch.h



librelacsauditorysession_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditorysession_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../../hardware/librelacsnieserieshardware.la \
    librelacsauditorytraces.la

$(librelacsauditorysession_la_OBJECTS) : \
    moc_auditorysession.cc

librelacsauditorysession_la_SOURCES = \
    auditorysession.cc \
    ../include/relacs/auditorysession.h



librelacsauditorytraces_la_CPPFLAGS = \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditorytraces_la_LIBADD = \
    ../../relacs/src/librelacs.la

librelacsauditorytraces_la_SOURCES = \
    auditorytraces.cc \
    ../include/relacs/auditorytraces.h



librelacsauditoryficurve_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryficurve_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorytraces.la \
    librelacsauditorysession.la

$(librelacsauditoryficurve_la_OBJECTS) : \
    moc_ficurve.cc

librelacsauditoryficurve_la_SOURCES = \
    ficurve.cc \
    ../include/relacs/ficurve.h



librelacsauditoryfifield_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryfifield_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorytraces.la \
    librelacsauditoryficurve.la

$(librelacsauditoryfifield_la_OBJECTS) : \
    moc_fifield.cc

librelacsauditoryfifield_la_SOURCES = \
    fifield.cc \
    ../include/relacs/fifield.h



librelacsauditorysinglestimulus_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditorysinglestimulus_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la

$(librelacsauditorysinglestimulus_la_OBJECTS) : \
    moc_singlestimulus.cc

librelacsauditorysinglestimulus_la_SOURCES = \
    singlestimulus.cc \
    ../include/relacs/singlestimulus.h



librelacsauditoryspikeprecision_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditoryspikeprecision_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la

$(librelacsauditoryspikeprecision_la_OBJECTS) : \
    moc_spikeprecision.cc

librelacsauditoryspikeprecision_la_SOURCES = \
    spikeprecision.cc \
    ../include/relacs/spikeprecision.h



librelacsauditorysyslatency_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../config/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../mplot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsauditorysyslatency_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la

$(librelacsauditorysyslatency_la_OBJECTS) : \
    moc_syslatency.cc

librelacsauditorysyslatency_la_SOURCES = \
    syslatency.cc \
    ../include/relacs/syslatency.h



if RELACS_LINKTESTS
linktest_librelacsauditoryneuron_la_SOURCES =
linktest_librelacsauditoryneuron_la_LDADD = librelacsauditoryneuron.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditorysearch_la_SOURCES =
linktest_librelacsauditorysearch_la_LDADD = librelacsauditorysearch.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditorysession_la_SOURCES =
linktest_librelacsauditorysession_la_LDADD = librelacsauditorysession.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditorytraces_la_SOURCES =
linktest_librelacsauditorytraces_la_LDADD = librelacsauditorytraces.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryficurve_la_SOURCES =
linktest_librelacsauditoryficurve_la_LDADD = librelacsauditoryficurve.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryfifield_la_SOURCES =
linktest_librelacsauditoryfifield_la_LDADD = librelacsauditoryfifield.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditorysinglestimulus_la_SOURCES =
linktest_librelacsauditorysinglestimulus_la_LDADD = librelacsauditorysinglestimulus.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditoryspikeprecision_la_SOURCES =
linktest_librelacsauditoryspikeprecision_la_LDADD = librelacsauditoryspikeprecision.la ../../relacs/src/librelacslinktest.la

linktest_librelacsauditorysyslatency_la_SOURCES =
linktest_librelacsauditorysyslatency_la_LDADD = librelacsauditorysyslatency.la ../../relacs/src/librelacslinktest.la
endif

