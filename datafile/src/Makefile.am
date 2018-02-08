lib_LTLIBRARIES = \
    librelacsdatafile.la


if RELACS_TOP_BUILD
librelacsdatafile_la_CPPFLAGS = \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include
else
librelacsdatafile_la_CPPFLAGS = \
    -I$(srcdir)/../include
endif

librelacsdatafile_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(GSL_LIBS)

if RELACS_TOP_BUILD
librelacsdatafile_la_LIBADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la
else
librelacsdatafile_la_LIBADD = \
    -lrelacsshapes \
    -lrelacsnumerics \
    -lrelacsoptions
endif

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
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
TESTS = $(check_PROGRAMS)

