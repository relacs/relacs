noinst_PROGRAMS = \
    copydata \
    processdata \
    xdatafile \
    xtablekey \
    xtranslate \
    pipe


AM_CPPFLAGS = \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../shapes/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

LDADD = \
    ../src/librelacsdatafile.la \
    ../../options/src/librelacsoptions.la \
    ../../shapes/src/librelacsshapes.la \
    ../../numerics/src/librelacsnumerics.la \
    $(GSL_LIBS)

copydata_SOURCES = copydata.cc

processdata_SOURCES = processdata.cc

xdatafile_SOURCES = xdatafile.cc

xtablekey_SOURCES = xtablekey.cc

xtranslate_SOURCES = xtranslate.cc

pipe_CPPFLAGS = 
pipe_LDADD = 
pipe_SOURCES = pipe.cc
