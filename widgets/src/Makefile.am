MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacswidgets.la



librelacswidgets_la_CPPFLAGS = \
    -I../../options/src \
    @QT3_CPPFLAGS@

librelacswidgets_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacswidgets_la-%-moc.cpp : %.h
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
    doublespinbox.h \
    lcdrange.h \
    messagebox.h \
    optdialog.h \
    optwidgetbase.h \
    optwidget.h \
    \
    optdialogbutton.h

librelacswidgets_la_SOURCES = \
    doublespinbox.cc \
    lcdrange.cc \
    messagebox.cc \
    optdialog.cc \
    optwidget.cc

