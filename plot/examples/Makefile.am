noinst_PROGRAMS = \
    xplot \
    xplots \
    xsurfaceplot


AM_CPPFLAGS = \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

AM_LDFLAGS = \
    $(QT4_LDFLAGS)

xplot_LDADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../daq/src/librelacsdaq.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS) $(GSL_LIBS)
xplot_SOURCES = xplot.cc

xplots_LDADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../daq/src/librelacsdaq.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS) $(GSL_LIBS)
xplots_SOURCES = xplots.cc

xsurfaceplot_LDADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../daq/src/librelacsdaq.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS) $(GSL_LIBS)
xsurfaceplot_SOURCES = xsurfaceplot.cc
