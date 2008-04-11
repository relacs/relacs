include ${top_srcdir}/moc3.mk

lib_LTLIBRARIES = \
    librelacsconfig.la


if RELACS_TOP_BUILD
librelacsconfig_la_CPPFLAGS = \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../../widgets/include \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
else
librelacsconfig_la_CPPFLAGS = \
    -I$(srcdir)/../include \
    $(QT3_CPPFLAGS)
endif

librelacsconfig_la_LDFLAGS = \
    $(QT3_LDFLAGS)

if RELACS_TOP_BUILD
librelacsconfig_la_LIBADD = \
    ../../options/src/librelacsoptions.la \
    ../../widgets/src/librelacswidgets.la \
    $(QT3_LIBS)
else
librelacsconfig_la_LIBADD = \
    -lrelacsoptions \
    -lrelacswidgets \
    $(QT3_LIBS)
endif

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


check_PROGRAMS = linktest_librelacsconfig_la
linktest_librelacsconfig_la_SOURCES = linktest.cc
linktest_librelacsconfig_la_LDADD = librelacsconfig.la
TESTS = $(check_PROGRAMS)

