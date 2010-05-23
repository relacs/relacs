HEADER_PATH = .
include ${top_srcdir}/moc4.mk

noinst_PROGRAMS = \
    xoptwidget


AM_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

AM_LDFLAGS = \
    $(QT4_LDFLAGS)


xoptwidget_LDADD = \
    ../../options/src/librelacsoptions.la \
    ../src/librelacswidgets.la \
    $(QT4_LIBS) \
    $(MATH_LIBS)
xoptwidget_SOURCES = xoptwidget.cc mainwidget.h
$(xoptwidget_OBJECTS) : moc_mainwidget.cc

