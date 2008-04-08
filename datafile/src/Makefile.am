lib_LTLIBRARIES = \
    librelacsdatafile.la


if RELACS_TOP_BUILD
librelacsdatafile_la_CPPFLAGS = \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include
else
librelacsdatafile_la_CPPFLAGS = \
    -I$(srcdir)/../include
endif

if RELACS_TOP_BUILD
librelacsdatafile_la_LIBADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la
else
librelacsdatafile_la_LIBADD = \
    -lrelacsnumerics \
    -lrelacsoptions
endif

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


check_PROGRAMS = linktest_librelacsdatafile_la
linktest_librelacsdatafile_la_SOURCES = linktest.cc
linktest_librelacsdatafile_la_LDADD = librelacsdatafile.la

