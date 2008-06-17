HEADER_PATH = .
include ${top_srcdir}/moc3.mk

noinst_PROGRAMS = \
    dsb \
    xoptwidget


AM_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)

AM_LDFLAGS = \
    $(QT3_LDFLAGS)

dsb_LDADD = \
    ../../options/src/librelacsoptions.la \
    ../src/librelacswidgets.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
dsb_SOURCES = dsb.cc

xoptwidget_LDADD = \
    ../../options/src/librelacsoptions.la \
    ../src/librelacswidgets.la \
    $(QT3_LIBS) \
    $(MATH_LIBS)
xoptwidget_SOURCES = xoptwidget.cc mainwidget.h
$(xoptwidget_OBJECTS) : moc_mainwidget.cc

