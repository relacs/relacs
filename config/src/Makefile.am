MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsconfig.la



librelacsconfig_la_CPPFLAGS = \
    -I../../options/include \
    -I../../widgets/include \
    -I../include \
    @QT3_CPPFLAGS@

librelacsconfig_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsconfig_la-%-moc.cpp : ../include/relacs/%.h
	$(MOC_CREATE)

librelacsconfig_la-%-moc.lo : librelacsconfig_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsconfig_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsconfig_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsconfig_la_LIBADD = \
    librelacsconfig_la-configdialog-moc.lo
    
librelacsconfig_ladir = $(includedir)/relacs

librelacsconfig_la_HEADERS = \
    ../include/relacs/config.h \
    ../include/relacs/configdialog.h \
    ../include/relacs/configure.h

librelacsconfig_la_SOURCES = \
    config.cc \
    configdialog.cc \
    configure.cc

