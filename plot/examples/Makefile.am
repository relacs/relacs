noinst_PROGRAMS = \
    xplot \
    xplots


AM_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

AM_LDFLAGS = \
    $(QT3_LDFLAGS)

xplot_LDADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
xplot_SOURCES = xplot.cc

xplots_LDADD = \
    ../../daq/src/librelacsdaq.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsplot.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
xplots_SOURCES = xplots.cc
