HEADER_PATH = ../include/relacs/hardware

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
    $(HEADER_PATH)/cs3310pp.h \
    $(HEADER_PATH)/kleindiek.h \
    $(HEADER_PATH)/tempdtm5080.h


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
    $(HEADER_PATH)/nidaq.h \
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
    $(HEADER_PATH)/amplmode.h \
    $(HEADER_PATH)/cs3310nidio.h \
    $(HEADER_PATH)/cs3310serialnidio.h \
    $(HEADER_PATH)/niai.h \
    $(HEADER_PATH)/niao.h \
    $(HEADER_PATH)/nidio.h \
    $(HEADER_PATH)/nipfi.h


check_PROGRAMS = \
    linktest_libhardware_la \
    linktest_libnieserieshardware_la

linktest_libhardware_la_SOURCES = linktest.cc
linktest_libhardware_la_LDADD = libhardware.la

linktest_libnieserieshardware_la_SOURCES = linktest.cc
linktest_libnieserieshardware_la_LDADD = libnieserieshardware.la

TESTS = $(check_PROGRAMS)

