lib_LTLIBRARIES = \
    librelacsnumerics.la



librelacsnumerics_la_LDFLAGS = \
    @GSL_LDFLAGS@

librelacsnumerics_la_CPPFLAGS = \
    -I../include \
    @GSL_CPPFLAGS@

librelacsnumerics_ladir = $(includedir)/relacs

librelacsnumerics_la_HEADERS = \
    ../include/relacs/array.h \
    ../include/relacs/basisfunction.h \
    ../include/relacs/eventdata.h \
    ../include/relacs/eventlist.h \
    ../include/relacs/fitalgorithm.h \
    ../include/relacs/kernel.h \
    ../include/relacs/linearrange.h \
    ../include/relacs/random.h \
    ../include/relacs/sampledata.h \
    ../include/relacs/spectrum.h \
    ../include/relacs/statstests.h \
    \
    ../include/relacs/containerops.h \
    ../include/relacs/containerfuncs.h \
    ../include/relacs/cyclicarray.h \
    ../include/relacs/cyclicsampledata.h \
    ../include/relacs/detector.h \
    ../include/relacs/map.h \
    ../include/relacs/odealgorithm.h \
    ../include/relacs/stats.h

librelacsnumerics_la_SOURCES = \
    array.cc \
    basisfunction.cc \
    eventdata.cc \
    eventlist.cc \
    fitalgorithm.cc \
    kernel.cc \
    linearrange.cc \
    random.cc \
    sampledata.cc \
    spectrum.cc \
    statstests.cc

