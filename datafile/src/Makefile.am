lib_LTLIBRARIES = \
    librelacsdatafile.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacsdatafile_la
endif



librelacsdatafile_la_CPPFLAGS = \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

librelacsdatafile_la_LIBADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la

librelacsdatafile_ladir = $(includedir)/relacs

librelacsdatafile_la_HEADERS = \
    ../include/relacs/datafile.h \
    ../include/relacs/tabledata.h \
    ../include/relacs/tablekey.h \
    ../include/relacs/translate.h

librelacsdatafile_la_SOURCES = \
    datafile.cc \
    tabledata.cc \
    tablekey.cc \
    translate.cc



if RELACS_LINKTESTS
linktest_librelacsdatafile_la_SOURCES =
linktest_librelacsdatafile_la_LDADD = librelacsdatafile.la ../../linktest/librelacslinktest.la
endif

