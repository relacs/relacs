include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsmplot.la


if RELACS_TOP_BUILD
librelacsmplot_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
else
librelacsmplot_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
endif

librelacsmplot_la_LDFLAGS = \
    $(QT3_LDFLAGS)

if RELACS_TOP_BUILD
librelacsmplot_la_LIBADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
else
librelacsmplot_la_LIBADD = \
    -lrelacsdaq \
    -lrelacsnumerics \
    -lrelacsoptions \
    $(QT3_LIBS) \
    $(MATH_LIBS)
endif

$(librelacsmplot_la_OBJECTS) : \
    moc_multiplot.cc \
    moc_plot.cc

librelacsmplot_ladir = $(includedir)/relacs

librelacsmplot_la_HEADERS = \
    ../include/relacs/multiplot.h \
    ../include/relacs/plot.h

librelacsmplot_la_SOURCES = \
    multiplot.cc \
    plot.cc


check_PROGRAMS = linktest_librelacsmplot_la
linktest_librelacsmplot_la_SOURCES = linktest.cc
linktest_librelacsmplot_la_LDADD = librelacsmplot.la
TESTS = $(check_PROGRAMS)

