include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsplot.la


if RELACS_TOP_BUILD
librelacsplot_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
else
librelacsplot_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
endif

librelacsplot_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(QT3_LDFLAGS)

if RELACS_TOP_BUILD
librelacsplot_la_LIBADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
else
librelacsplot_la_LIBADD = \
    -lrelacsdaq \
    -lrelacsnumerics \
    -lrelacsoptions \
    $(QT3_LIBS) \
    $(MATH_LIBS)
endif

$(librelacsplot_la_OBJECTS) : \
    moc_multiplot.cc \
    moc_plot.cc

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/multiplot.h \
    ../include/relacs/plot.h

librelacsplot_la_SOURCES = \
    multiplot.cc \
    plot.cc


check_PROGRAMS = linktest_librelacsplot_la
linktest_librelacsplot_la_SOURCES = linktest.cc
linktest_librelacsplot_la_LDADD = librelacsplot.la
TESTS = $(check_PROGRAMS)

