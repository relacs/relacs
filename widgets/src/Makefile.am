include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacswidgets.la


if RELACS_TOP_BUILD
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
else
librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
endif

librelacswidgets_la_LDFLAGS = \
    -version-info 0:0:0 \
    $(QT3_LDFLAGS)

if RELACS_TOP_BUILD
librelacswidgets_la_LIBADD = \
    ../../options/src/librelacsoptions.la \
    $(QT3_LIBS)
else
librelacswidgets_la_LIBADD = \
    -lrelacsoptions \
    $(QT3_LIBS)
endif

$(librelacswidgets_la_OBJECTS) : \
    moc_configdialog.cc \
    moc_doublespinbox.cc \
    moc_lcdrange.cc \
    moc_messagebox.cc \
    moc_optdialog.cc \
    moc_optdialogbutton.cc \
    moc_optwidget.cc \
    moc_optwidgetbase.cc

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/configdialog.h \
    ../include/relacs/doublespinbox.h \
    ../include/relacs/lcdrange.h \
    ../include/relacs/messagebox.h \
    ../include/relacs/optdialog.h \
    ../include/relacs/optwidgetbase.h \
    ../include/relacs/optwidget.h \
    \
    ../include/relacs/optdialogbutton.h

librelacswidgets_la_SOURCES = \
    configdialog.cc \
    doublespinbox.cc \
    lcdrange.cc \
    messagebox.cc \
    optdialog.cc \
    optwidget.cc \
    \
    optwidgetbase.cc \
    optdialogbutton.cc


check_PROGRAMS = linktest_librelacswidgets_la
linktest_librelacswidgets_la_SOURCES = linktest.cc
linktest_librelacswidgets_la_LDADD = librelacswidgets.la
TESTS = $(check_PROGRAMS)

