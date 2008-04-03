MOC = @MOC@
include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsconfig.la

if RELACS_LINKTESTS
noinst_PROGRAMS = \
    linktest_librelacsconfig_la
endif



librelacsconfig_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    @QT3_CPPFLAGS@

librelacsconfig_la_LDFLAGS = \
    @QT3_LDFLAGS@

librelacsconfig_la_LIBADD = \
    ../../options/src/librelacsoptions.la \
    ../../widgets/src/librelacswidgets.la \
    @QT3_LIBS@

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



if RELACS_LINKTESTS
linktest_librelacsconfig_la_SOURCES =
linktest_librelacsconfig_la_LDADD = librelacsconfig.la ../../linktest/librelacslinktest.la
endif

