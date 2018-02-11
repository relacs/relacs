lib_LTLIBRARIES = \
    librelacsshapes.la

librelacsshapes_la_CPPFLAGS = \
    -I$(srcdir)/../include

librelacsshapes_la_LDFLAGS = \
    -version-info 0:0:0

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/point.h \
    ../include/relacs/transform.h \
    ../include/relacs/shape.h

librelacsshapes_la_SOURCES = \
    point.cc \
    transform.cc \
    shape.cc


check_PROGRAMS = linktest_librelacsshapes_la \
    checkpoint \
    checktransform \
    checkshape

linktest_librelacsshapes_la_SOURCES = linktest.cc
linktest_librelacsshapes_la_LDADD = librelacsshapes.la

checkpoint_SOURCES = checkpoint.cc
checkpoint_CPPFLAGS = -I$(srcdir)/../include
checkpoint_LDADD = librelacsshapes.la

checktransform_SOURCES = checktransform.cc
checktransform_CPPFLAGS = -I$(srcdir)/../include
checktransform_LDADD = librelacsshapes.la

checkshape_SOURCES = checkshape.cc
checkshape_CPPFLAGS = -I$(srcdir)/../include
checkshape_LDADD = librelacsshapes.la

TESTS = $(check_PROGRAMS)

