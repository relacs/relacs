lib_LTLIBRARIES = \
    librelacsoptions.la

EXTRA_PROGRAMS = \
    linktest_librelacsoptions_la


librelacsoptions_ladir = $(includedir)/relacs

librelacsoptions_la_CPPFLAGS = \
    -I$(srcdir)/../include

librelacsoptions_la_HEADERS = \
    ../include/relacs/options.h \
    ../include/relacs/parameter.h \
    ../include/relacs/str.h \
    ../include/relacs/strqueue.h

librelacsoptions_la_SOURCES = \
    options.cc \
    parameter.cc \
    str.cc \
    strqueue.cc



if RELACS_LINKTESTS
noinst_PROGRAMS = linktest_librelacsoptions_la
linktest_librelacsoptions_la_SOURCES = linktest.cc
linktest_librelacsoptions_la_LDADD = librelacsoptions.la
endif

