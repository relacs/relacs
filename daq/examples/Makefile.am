noinst_PROGRAMS = \
    indatatimeindex


AM_CPPFLAGS = \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include


indatatimeindex_LDADD = \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdaq.la \
    $(GSL_LIBS)
indatatimeindex_SOURCES = indatatimeindex.cc
