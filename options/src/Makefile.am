lib_LTLIBRARIES = \
    librelacsoptions.la


librelacsoptions_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../shapes/include

librelacsoptions_la_LDFLAGS = \
    -version-info 0:0:0

librelacsoptions_la_LIBADD = \
    ../../shapes/src/librelacsshapes.la

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/str.h \
    ../include/relacs/strqueue.h \
    ../include/relacs/parameter.h \
    ../include/relacs/options.h \
    ../include/relacs/configclass.h \
    ../include/relacs/configureclasses.h

librelacsoptions_la_SOURCES = \
    str.cc \
    strqueue.cc \
    parameter.cc \
    options.cc \
    configclass.cc \
    configureclasses.cc


check_PROGRAMS = linktest_librelacsoptions_la
linktest_librelacsoptions_la_SOURCES = linktest.cc
linktest_librelacsoptions_la_LDADD = librelacsoptions.la
TESTS = $(check_PROGRAMS)

