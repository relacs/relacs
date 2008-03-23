MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsconfig.la



librelacsconfig_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsconfig_la_LDFLAGS = \
    @QT3_LDFLAGS@

$(librelacsconfig_la_OBJECTS) : \
    moc_configdialog.cc

librelacsconfig_ladir = $(includedir)/relacs

librelacsconfig_la_HEADERS = \
    ../include/relacs/config.h \
    ../include/relacs/configdialog.h \
    ../include/relacs/configure.h

librelacsconfig_la_SOURCES = \
    config.cc \
    configdialog.cc \
    configure.cc

