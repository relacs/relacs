MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacswidgets.la



librelacswidgets_la_CPPFLAGS = \
    -I../../options/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacswidgets_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacswidgets_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacswidgets_la-%-moc.lo : librelacswidgets_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacswidgets_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacswidgets_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacswidgets_la_LIBADD = \
    librelacswidgets_la-doublespinbox-moc.lo \
    librelacswidgets_la-lcdrange-moc.lo \
    librelacswidgets_la-messagebox-moc.lo \
    librelacswidgets_la-optdialog-moc.lo \
    librelacswidgets_la-optdialogbutton-moc.lo \
    librelacswidgets_la-optwidget-moc.lo \
    librelacswidgets_la-optwidgetbase-moc.lo

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

