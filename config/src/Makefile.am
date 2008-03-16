MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsconfig.la



librelacsconfig_la_CPPFLAGS = \
    -I../../options/src \
    -I../../widgets/src \
    @QT3_CPPFLAGS@

librelacsconfig_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsconfig_la-%-moc.cpp : %.h
	$(MOC_CREATE)

librelacsconfig_la-%-moc.lo : librelacsconfig_la-%-moc.cpp
	$(MOC_COMPILE_1) $(librelacsconfig_la_CPPFLAGS) \
	$(MOC_COMPILE_2) $(librelacsconfig_la_CXXFLAGS) \
	$(MOC_COMPILE_3)

librelacsconfig_la_LIBADD = \
    librelacsconfig_la-configdialog-moc.lo
    
librelacsconfig_ladir = $(includedir)/relacs

librelacsconfig_la_HEADERS = \
    configclass.h \
    configdialog.h \
    configure.h

librelacsconfig_la_SOURCES = \
    configclass.cc \
    configdialog.cc \
    configure.cc
