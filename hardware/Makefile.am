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
    -Iinclude \
    @QT3_CPPFLAGS@

librelacshardware_la_LIBADD = \
    ../relacs/src/librelacs.la

librelacshardware_la_SOURCES = \
    include/relacs/cs3310pp.h \
    include/relacs/kleindiek.h \
    include/relacs/tempdtm5080.h \
    \
    cs3310pp.cc \
    kleindiek.cc \
    tempdtm5080.cc \
    \
    hardware.cc



librelacsnieserieshardware_la_CPPFLAGS = \
    -DNIDAQ_MAJOR=254 \
    -I../config/include \
    -I../daq/include \
    -I../numerics/include \
    -I../options/include \
    -I../relacs/include \
    -Iinclude \
    @QT3_CPPFLAGS@

librelacsnieserieshardware_la_LIBADD = \
    ../relacs/src/librelacs.la

librelacsnieserieshardware_la_SOURCES = \
    include/relacs/amplmode.h \
    include/relacs/cs3310nidio.h \
    include/relacs/cs3310serialnidio.h \
    include/relacs/niai.h \
    include/relacs/niao.h \
    include/relacs/nidio.h \
    include/relacs/nipfi.h \
    \
    include/relacs/nidaq.h \
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



if RELACS_LINKTESTS
linktest_librelacshardware_la_SOURCES =
linktest_librelacshardware_la_LDADD = librelacshardware.la ../relacs/src/librelacslinktest.la

linktest_librelacsnieserieshardware_la_SOURCES =
linktest_librelacsnieserieshardware_la_LDADD = librelacsnieserieshardware.la ../relacs/src/librelacslinktest.la
endif

