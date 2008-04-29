include ${top_srcdir}/moc3.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libauditorytraces.la \
    libauditorysession.la \
    libauditoryficurve.la \
    libauditoryneuron.la \
    libauditorysearch.la \
    libauditoryfifield.la \
    libauditorysinglestimulus.la \
    libauditoryspikeprecision.la \
    libauditorysyslatency.la


libauditoryneuron_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryneuron_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryneuron_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/models/libcommonneuronmodels.la \
    $(QT3_LIBS)

$(libauditoryneuron_la_OBJECTS) : \
    moc_auditoryneuron.cc

libauditoryneuron_la_SOURCES = \
    auditoryneuron.cc

libauditoryneuron_la_includedir = $(pkgincludedir)/auditory

libauditoryneuron_la_include_HEADERS = \
    ../include/relacs/auditoryneuron.h



libauditorysearch_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorysearch_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorysearch_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    libauditorytraces.la \
    $(QT3_LIBS)

$(libauditorysearch_la_OBJECTS) : \
    moc_auditorysearch.cc

libauditorysearch_la_SOURCES = \
    auditorysearch.cc

libauditorysearch_la_includedir = $(pkgincludedir)/auditory

libauditorysearch_la_include_HEADERS = \
    ../include/relacs/auditorysearch.h



libauditorysession_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorysession_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorysession_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    ../../hardware/src/libnieserieshardware.la \
    libauditorytraces.la \
    $(QT3_LIBS)

$(libauditorysession_la_OBJECTS) : \
    moc_auditorysession.cc

libauditorysession_la_SOURCES = \
    auditorysession.cc

libauditorysession_la_includedir = $(pkgincludedir)/auditory

libauditorysession_la_include_HEADERS = \
    ../include/relacs/auditorysession.h



libauditorytraces_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorytraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorytraces_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

libauditorytraces_la_SOURCES = \
    auditorytraces.cc

libauditorytraces_la_includedir = $(pkgincludedir)/auditory

libauditorytraces_la_include_HEADERS = \
    ../include/relacs/auditorytraces.h



libauditoryficurve_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryficurve_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryficurve_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    libauditorytraces.la \
    libauditorysession.la \
    $(QT3_LIBS)

$(libauditoryficurve_la_OBJECTS) : \
    moc_ficurve.cc

libauditoryficurve_la_SOURCES = \
    ficurve.cc

libauditoryficurve_la_includedir = $(pkgincludedir)/auditory

libauditoryficurve_la_include_HEADERS = \
    ../include/relacs/ficurve.h



libauditoryfifield_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryfifield_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryfifield_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    libauditorytraces.la \
    libauditoryficurve.la \
    $(QT3_LIBS)

$(libauditoryfifield_la_OBJECTS) : \
    moc_fifield.cc

libauditoryfifield_la_SOURCES = \
    fifield.cc

libauditoryfifield_la_includedir = $(pkgincludedir)/auditory

libauditoryfifield_la_include_HEADERS = \
    ../include/relacs/fifield.h



libauditorysinglestimulus_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorysinglestimulus_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorysinglestimulus_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    libauditorysession.la \
    libauditorytraces.la \
    $(QT3_LIBS)

$(libauditorysinglestimulus_la_OBJECTS) : \
    moc_singlestimulus.cc

libauditorysinglestimulus_la_SOURCES = \
    singlestimulus.cc

libauditorysinglestimulus_la_includedir = $(pkgincludedir)/auditory

libauditorysinglestimulus_la_include_HEADERS = \
    ../include/relacs/singlestimulus.h



libauditoryspikeprecision_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditoryspikeprecision_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditoryspikeprecision_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    libauditorysession.la \
    libauditorytraces.la \
    $(QT3_LIBS)

$(libauditoryspikeprecision_la_OBJECTS) : \
    moc_spikeprecision.cc

libauditoryspikeprecision_la_SOURCES = \
    spikeprecision.cc

libauditoryspikeprecision_la_includedir = $(pkgincludedir)/auditory

libauditoryspikeprecision_la_include_HEADERS = \
    ../include/relacs/spikeprecision.h



libauditorysyslatency_la_CPPFLAGS = \
    -I$(srcdir)/../../common/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../datafile/include \
    -I$(srcdir)/../../hardware/include \
    -I$(srcdir)/../../plot/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libauditorysyslatency_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libauditorysyslatency_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    ../../common/base/libcommonephystraces.la \
    libauditorysession.la \
    libauditorytraces.la \
    $(QT3_LIBS)

$(libauditorysyslatency_la_OBJECTS) : \
    moc_syslatency.cc

libauditorysyslatency_la_SOURCES = \
    syslatency.cc

libauditorysyslatency_la_includedir = $(pkgincludedir)/auditory

libauditorysyslatency_la_include_HEADERS = \
    ../include/relacs/syslatency.h



check_PROGRAMS = \
    linktest_libauditoryneuron_la \
    linktest_libauditorysearch_la \
    linktest_libauditorysession_la \
    linktest_libauditorytraces_la \
    linktest_libauditoryficurve_la \
    linktest_libauditoryfifield_la \
    linktest_libauditorysinglestimulus_la \
    linktest_libauditoryspikeprecision_la \
    linktest_libauditorysyslatency_la

linktest_libauditoryneuron_la_SOURCES = linktest.cc
linktest_libauditoryneuron_la_LDADD = libauditoryneuron.la

linktest_libauditorysearch_la_SOURCES = linktest.cc
linktest_libauditorysearch_la_LDADD = libauditorysearch.la

linktest_libauditorysession_la_SOURCES = linktest.cc
linktest_libauditorysession_la_LDADD = libauditorysession.la

linktest_libauditorytraces_la_SOURCES = linktest.cc
linktest_libauditorytraces_la_LDADD = libauditorytraces.la

linktest_libauditoryficurve_la_SOURCES = linktest.cc
linktest_libauditoryficurve_la_LDADD = libauditoryficurve.la

linktest_libauditoryfifield_la_SOURCES = linktest.cc
linktest_libauditoryfifield_la_LDADD = libauditoryfifield.la

linktest_libauditorysinglestimulus_la_SOURCES = linktest.cc
linktest_libauditorysinglestimulus_la_LDADD = libauditorysinglestimulus.la

linktest_libauditoryspikeprecision_la_SOURCES = linktest.cc
linktest_libauditoryspikeprecision_la_LDADD = libauditoryspikeprecision.la

linktest_libauditorysyslatency_la_SOURCES = linktest.cc
linktest_libauditorysyslatency_la_LDADD = libauditorysyslatency.la

TESTS = $(check_PROGRAMS)

