MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacswidgets.la



librelacswidgets_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacswidgets_la_LDFLAGS = \
    @QT3_LDFLAGS@

$(librelacswidgets_la_OBJECTS) : \
    moc_doublespinbox.cc \
    moc_lcdrange.cc \
    moc_messagebox.cc \
    moc_optdialog.cc \
    moc_optdialogbutton.cc \
    moc_optwidget.cc \
    moc_optwidgetbase.cc

librelacswidgets_ladir = $(includedir)/relacs

librelacswidgets_la_HEADERS = \
    ../include/relacs/doublespinbox.h \
    ../include/relacs/lcdrange.h \
    ../include/relacs/messagebox.h \
    ../include/relacs/optdialog.h \
    ../include/relacs/optwidgetbase.h \
    ../include/relacs/optwidget.h \
    \
    ../include/relacs/optdialogbutton.h

librelacswidgets_la_SOURCES = \
    doublespinbox.cc \
    lcdrange.cc \
    messagebox.cc \
    optdialog.cc \
    optwidget.cc

