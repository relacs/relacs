MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsmplot.la



librelacsmplot_la_CPPFLAGS = \
    -I../../daq/src \
    -I../../numerics/src \
    -I../../options/src \
    -I. \
    @QT3_CPPFLAGS@

librelacsmplot_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsmplot_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacsmplot_la-%-moc.lo : librelacsmplot_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsmplot_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsmplot_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsmplot_la_LIBADD = \
    librelacsmplot_la-multiplot-moc.lo \
    librelacsmplot_la-plot-moc.lo

librelacsmplot_ladir = $(includedir)/relacs

librelacsmplot_la_HEADERS = \
    multiplot.h \
    plot.h

librelacsmplot_la_SOURCES = \
    multiplot.cc \
    plot.cc

