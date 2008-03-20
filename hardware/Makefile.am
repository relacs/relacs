libdir = $(prefix)/lib/@PLUGINS_BASENAME@

lib_LTLIBRARIES = \
    librelacshardware.la \
    librelacsnieserieshardware.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacshardware_la \
    linktest_librelacsnieserieshardware_la
endif



librelacshardware_la_CPPFLAGS = \
    -I../config/include \
    -I../daq/include \
    -I../numerics/include \
    -I../options/include \
    -I../relacs/include \
    @QT3_CPPFLAGS@

librelacshardware_la_LIBADD = \
    ../relacs/src/librelacs.la

librelacshardware_la_SOURCES = \
    cs3310pp.cc \
    ../include/relacs/cs3310pp.h \
    hardware.cc \
    kleindiek.cc \
    ../include/relacs/kleindiek.h \
    tempdtm5080.cc \
    ../include/relacs/tempdtm5080.h



librelacsnieserieshardware_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=254 \
    -I../config/include \
    -I../daq/include \
    -I../numerics/include \
    -I../options/include \
    -I../relacs/include \
    @QT3_CPPFLAGS@

librelacsnieserieshardware_la_LIBADD = \
    ../relacs/src/librelacs.la

librelacsnieserieshardware_la_SOURCES = \
    amplmode.cc \
    ../include/relacs/amplmode.h \
    cs3310nidio.cc \
    ../include/relacs/cs3310nidio.h \
    cs3310serialnidio.cc \
    ../include/relacs/cs3310serialnidio.h \
    niai.cc \
    ../include/relacs/niai.h \
    niao.cc \
    ../include/relacs/niao.h \
    ../include/relacs/nidaq.h \
    nidio.cc \
    ../include/relacs/nidio.h \
    nieserieshardware.cc \
    nipfi.cc \
    ../include/relacs/nipfi.h



if RELACS_LINKTESTS
linktest_librelacshardware_la_SOURCES =
linktest_librelacshardware_la_LDADD = librelacshardware.la ../relacs/src/librelacslinktest.la

linktest_librelacsnieserieshardware_la_SOURCES =
linktest_librelacsnieserieshardware_la_LDADD = librelacsnieserieshardware.la ../relacs/src/librelacslinktest.la
endif

