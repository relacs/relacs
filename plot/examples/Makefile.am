HEADER_PATH = .
include ${top_srcdir}/moc4.mk

noinst_PROGRAMS = \
    xplot \
    xplots \
    xsurfaceplot \
    xshapeplot \
    xzoneplot

AM_CPPFLAGS = \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) -DHAVE_RELACSLIBSHAPES

AM_LDFLAGS = \
    $(QT4_LDFLAGS)

LDADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../daq/src/librelacsdaq.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT4_LIBS) \
    $(MATH_LIBS) $(GSL_LIBS)

xplot_SOURCES = xplot.cc

xplots_SOURCES = xplots.cc

xsurfaceplot_SOURCES = xsurfaceplot.cc

xshapeplot_SOURCES = xshapeplot.cc xshapeplot.h

$(xshapeplot_OBJECTS) : \
    moc_xshapeplot.cc

xzoneplot_SOURCES = xzoneplot.cc xzoneplot.h

$(xzoneplot_OBJECTS) : \
    moc_xzoneplot.cc
