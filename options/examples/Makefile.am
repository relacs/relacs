noinst_PROGRAMS = \
    xoptions \
    xparameter \
    xstring \
    xstrnum \
    xstrdatetime \
    xunit


AM_CPPFLAGS = \
    -I$(srcdir)/../include \
    -I$(srcdir)/../../daq/include

LDADD = \
    ../src/librelacsoptions.la \
    ../../daq/src/librelacsdaq.la


xoptions_SOURCES = xoptions.cc
xparameter_SOURCES = xparameter.cc
xstring_SOURCES = xstring.cc
xstrnum_SOURCES = xstrnum.cc
xstrdatetime_SOURCES = xstrdatetime.cc
xunit_SOURCES = xunit.cc
