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
    -I../../common/models \
    -I../../config/include \
    -I../../daq/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditoryneuron_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditoryneuron_la-%-moc.lo : librelacsauditoryneuron_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryneuron_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryneuron_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryneuron_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/models/librelacscommonneuronmodels.la \
    librelacsauditoryneuron_la-auditoryneuron-moc.lo

librelacsauditoryneuron_la_SOURCES = \
    auditoryneuron.cc \
    auditoryneuron.h



librelacsauditorysearch_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditorysearch_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditorysearch_la-%-moc.lo : librelacsauditorysearch_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditorysearch_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditorysearch_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditorysearch_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    librelacsauditorytraces.la \
    librelacsauditorysearch_la-auditorysearch-moc.lo

librelacsauditorysearch_la_SOURCES = \
    auditorysearch.cc \
    auditorysearch.h



librelacsauditorysession_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditorysession_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditorysession_la-%-moc.lo : librelacsauditorysession_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditorysession_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditorysession_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditorysession_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    ../../hardware/librelacsnieserieshardware.la \
    librelacsauditorytraces.la \
    librelacsauditorysession_la-auditorysession-moc.lo

librelacsauditorysession_la_SOURCES = \
    auditorysession.cc \
    auditorysession.h



librelacsauditorytraces_la_CPPFLAGS = \
    -I../../config/include \
    -I../../daq/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    @QT3_CPPFLAGS@

librelacsauditorytraces_la_LIBADD = \
    ../../relacs/src/librelacs.la

librelacsauditorytraces_la_SOURCES = \
    auditorytraces.cc \
    auditorytraces.h



librelacsauditoryficurve_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditoryficurve_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditoryficurve_la-%-moc.lo : librelacsauditoryficurve_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryficurve_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryficurve_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryficurve_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorytraces.la \
    librelacsauditorysession.la \
    librelacsauditoryficurve_la-ficurve-moc.lo

librelacsauditoryficurve_la_SOURCES = \
    ficurve.cc \
    ficurve.h



librelacsauditoryfifield_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditoryfifield_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditoryfifield_la-%-moc.lo : librelacsauditoryfifield_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryfifield_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryfifield_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryfifield_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorytraces.la \
    librelacsauditoryficurve.la \
    librelacsauditoryfifield_la-fifield-moc.lo

librelacsauditoryfifield_la_SOURCES = \
    fifield.cc \
    fifield.h



librelacsauditorysinglestimulus_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditorysinglestimulus_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditorysinglestimulus_la-%-moc.lo : librelacsauditorysinglestimulus_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditorysinglestimulus_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditorysinglestimulus_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditorysinglestimulus_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la \
    librelacsauditorysinglestimulus_la-singlestimulus-moc.lo

librelacsauditorysinglestimulus_la_SOURCES = \
    singlestimulus.cc \
    singlestimulus.h



librelacsauditoryspikeprecision_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditoryspikeprecision_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditoryspikeprecision_la-%-moc.lo : librelacsauditoryspikeprecision_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditoryspikeprecision_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditoryspikeprecision_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditoryspikeprecision_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la \
    librelacsauditoryspikeprecision_la-spikeprecision-moc.lo

librelacsauditoryspikeprecision_la_SOURCES = \
    spikeprecision.cc \
    spikeprecision.h



librelacsauditorysyslatency_la_CPPFLAGS = \
    -I../../common/base \
    -I../../config/include \
    -I../../daq/include \
    -I../../datafile/include \
    -I../../hardware \
    -I../../mplot/include \
    -I../../numerics/include \
    -I../../options/include \
    -I../../relacs/include/relacs/ \
    -I../../widgets/include \
    @QT3_CPPFLAGS@

librelacsauditorysyslatency_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsauditorysyslatency_la-%-moc.lo : librelacsauditorysyslatency_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsauditorysyslatency_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsauditorysyslatency_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsauditorysyslatency_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/librelacscommonephystraces.la \
    librelacsauditorysession.la \
    librelacsauditorytraces.la \
    librelacsauditorysyslatency_la-syslatency-moc.lo

librelacsauditorysyslatency_la_SOURCES = \
    syslatency.cc \
    syslatency.h



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

