pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libhardware.la \
    libnieserieshardware.la


libhardware_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libhardware_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libhardware_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

libhardware_la_SOURCES = \
    cs3310pp.cc \
    kleindiek.cc \
    tempdtm5080.cc \
    \
    hardware.cc

libhardware_la_includedir = $(pkgincludedir)/hardware

libhardware_la_include_HEADERS = \
    ../include/relacs/cs3310pp.h \
    ../include/relacs/kleindiek.h \
    ../include/relacs/tempdtm5080.h


libnieserieshardware_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=254 \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../relacs/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

libnieserieshardware_la_LDFLAGS = \
    -module -avoid-version \
    $(QT3_LDFLAGS)

libnieserieshardware_la_LIBADD = \
    ../../relacs/src/librelacs.la \
    $(QT3_LIBS)

libnieserieshardware_la_SOURCES = \
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

libnieserieshardware_la_includedir = $(pkgincludedir)/hardware

libnieserieshardware_la_include_HEADERS = \
    ../include/relacs/amplmode.h \
    ../include/relacs/cs3310nidio.h \
    ../include/relacs/cs3310serialnidio.h \
    ../include/relacs/niai.h \
    ../include/relacs/niao.h \
    ../include/relacs/nidio.h \
    ../include/relacs/nipfi.h


check_PROGRAMS = \
    linktest_libhardware_la \
    linktest_libnieserieshardware_la

linktest_libhardware_la_SOURCES = linktest.cc
linktest_libhardware_la_LDADD = libhardware.la

linktest_libnieserieshardware_la_SOURCES = linktest.cc
linktest_libnieserieshardware_la_LDADD = libnieserieshardware.la

TESTS = $(check_PROGRAMS)

