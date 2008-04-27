libdir = $(prefix)/lib/$(PLUGINS_BASENAME)

lib_LTLIBRARIES = \
    librelacshardware.la \
    librelacsnieserieshardware.la


librelacshardware_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

librelacshardware_la_LDFLAGS = \
    $(QT3_LDFLAGS)

librelacshardware_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

librelacshardware_la_SOURCES = \
    ../include/relacs/cs3310pp.h \
    ../include/relacs/kleindiek.h \
    ../include/relacs/tempdtm5080.h \
    \
    cs3310pp.cc \
    kleindiek.cc \
    tempdtm5080.cc \
    \
    hardware.cc



librelacsnieserieshardware_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=254 \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

librelacsnieserieshardware_la_LDFLAGS = \
    $(QT3_LDFLAGS)

librelacsnieserieshardware_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

librelacsnieserieshardware_la_SOURCES = \
    ../include/relacs/amplmode.h \
    ../include/relacs/cs3310nidio.h \
    ../include/relacs/cs3310serialnidio.h \
    ../include/relacs/niai.h \
    ../include/relacs/niao.h \
    ../include/relacs/nidio.h \
    ../include/relacs/nipfi.h \
    \
    ../include/relacs/nidaq.h \
    \
    amplmode.cc \
    cs3310nidio.cc \
    cs3310serialnidio.cc \
    niai.cc \
    niao.cc \
    nidio.cc \
    nipfi.cc \
    \
    nieserieshardware.cc


check_PROGRAMS = \
    linktest_librelacshardware_la \
    linktest_librelacsnieserieshardware_la

linktest_librelacshardware_la_SOURCES = linktest.cc
linktest_librelacshardware_la_LDADD = librelacshardware.la

linktest_librelacsnieserieshardware_la_SOURCES = linktest.cc
linktest_librelacsnieserieshardware_la_LDADD = librelacsnieserieshardware.la

TESTS = $(check_PROGRAMS)

