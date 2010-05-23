HEADER_PATH = ../include/relacs
include ${top_srcdir}/moc4.mk

lib_LTLIBRARIES = \
    librelacswidgets.la


if RELACS_TOP_BUILD
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)
else
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)
endif

librelacswidgets_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(QT4_LDFLAGS)

if RELACS_TOP_BUILD
librelacswidgets_la_LIBADD = \
    ../../options/src/librelacsoptions.la \
    $(QT4_LIBS)
else
librelacswidgets_la_LIBADD = \
    -lrelacsoptions \
    $(QT4_LIBS)
endif

$(librelacswidgets_la_OBJECTS) : \
    moc_configdialog.cc \
    moc_optdialog.cc \
    moc_optdialogbutton.cc \
    moc_optwidget.cc \
    moc_optwidgetbase.cc \
    moc_lcdrange.cc \
    moc_messagebox.cc

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/configdialog.h \
    ../include/relacs/optdialog.h \
    ../include/relacs/optwidget.h \
    ../include/relacs/lcdrange.h \
    ../include/relacs/messagebox.h

librelacswidgets_la_SOURCES = \
    configdialog.cc \
    optdialog.cc \
    optwidget.cc \
    ../include/relacs/optwidgetbase.h \
    optwidgetbase.cc \
    ../include/relacs/optdialogbutton.h \
    optdialogbutton.cc \
    lcdrange.cc \
    messagebox.cc


check_PROGRAMS = linktest_librelacswidgets_la
linktest_librelacswidgets_la_SOURCES = linktest.cc
linktest_librelacswidgets_la_LDADD = librelacswidgets.la
TESTS = $(check_PROGRAMS)

