MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsmplot.la



librelacsmplot_la_CPPFLAGS = \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsmplot_la_LDFLAGS = \
    @QT3_LDFLAGS@

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

