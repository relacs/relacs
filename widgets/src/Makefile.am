HEADER_PATH = ../include/relacs
include ${top_srcdir}/moc4.mk

lib_LTLIBRARIES = \
    librelacswidgets.la


if RELACS_TOP_BUILD
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS)
else
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS)
endif

librelacswidgets_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(QT_LDFLAGS)

if RELACS_TOP_BUILD
librelacswidgets_la_LIBADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../options/src/librelacsoptions.la \
    $(QT_LIBS)
else
librelacswidgets_la_LIBADD = \
    -lrelacsshapes \
    -lrelacsdaq \
    -lrelacsoptions \
    $(QT_LIBS)
endif

$(librelacswidgets_la_OBJECTS) : \
    moc_configdialog.cc \
    moc_optdialog.cc \
    moc_optdialogbutton.cc \
    moc_optwidget.cc \
    moc_optwidgetbase.cc \
    moc_doublespinbox.cc \
    moc_lcdrange.cc \
    moc_messagebox.cc \
    moc_scaledimage.cc

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/configdialog.h \
    ../include/relacs/optdialog.h \
    ../include/relacs/optwidget.h \
    ../include/relacs/doublespinbox.h \
    ../include/relacs/lcdrange.h \
    ../include/relacs/messagebox.h \
    ../include/relacs/scaledimage.h

librelacswidgets_la_SOURCES = \
    configdialog.cc \
    optdialog.cc \
    optwidget.cc \
    ../include/relacs/optwidgetbase.h \
    optwidgetbase.cc \
    ../include/relacs/optdialogbutton.h \
    optdialogbutton.cc \
    doublespinbox.cc \
    lcdrange.cc \
    messagebox.cc \
    scaledimage.cc


check_PROGRAMS = linktest_librelacswidgets_la
linktest_librelacswidgets_la_SOURCES = linktest.cc
linktest_librelacswidgets_la_LDADD = librelacswidgets.la
TESTS = $(check_PROGRAMS)

