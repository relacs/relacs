lib_LTLIBRARIES = \
    librelacsshapes.la

librelacsshapes_la_CPPFLAGS = \
    -I$(srcdir)/../include

librelacsshapes_la_LDFLAGS = \
    -version-info 0:0:0

pkgincludedir = $(includedir)/relacs

pkginclude_HEADERS = \
    ../include/relacs/point.h \
    ../include/relacs/matrix.h \
    ../include/relacs/shape.h

librelacsshapes_la_SOURCES = \
    point.cc \
    matrix.cc \
    shape.cc


check_PROGRAMS = linktest_librelacsshapes_la \
    checkpoint \
    checkmatrix \
    checkshape

linktest_librelacsshapes_la_SOURCES = linktest.cc
linktest_librelacsshapes_la_LDADD = librelacsshapes.la

checkpoint_SOURCES = checkpoint.cc
checkpoint_CPPFLAGS = -I$(srcdir)/../include
checkpoint_LDADD = librelacsshapes.la

checkmatrix_SOURCES = checkmatrix.cc
checkmatrix_CPPFLAGS = -I$(srcdir)/../include
checkmatrix_LDADD = librelacsshapes.la

checkshape_SOURCES = checkshape.cc
checkshape_CPPFLAGS = -I$(srcdir)/../include
checkshape_LDADD = librelacsshapes.la

TESTS = $(check_PROGRAMS)

