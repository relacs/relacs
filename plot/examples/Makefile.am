noinst_PROGRAMS = \
    xplot \
    xplots


AM_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

AM_LDFLAGS = \
    $(QT4_LDFLAGS)

xplot_LDADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS)
xplot_SOURCES = xplot.cc

xplots_LDADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS)
xplots_SOURCES = xplots.cc
