noinst_PROGRAMS = \
    copydata \
    pipe \
    processdata \
    xdatafile \
    xtablekey \
    xtranslate


AM_CPPFLAGS = \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include

copydata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la $(GSL_LIBS)
copydata_SOURCES = copydata.cc

pipe_CPPFLAGS = 
pipe_LDADD = 
pipe_SOURCES = pipe.cc

processdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la $(GSL_LIBS)
processdata_SOURCES = processdata.cc

xdatafile_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la $(GSL_LIBS)
xdatafile_SOURCES = xdatafile.cc

xtablekey_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la $(GSL_LIBS)
xtablekey_SOURCES = xtablekey.cc

xtranslate_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la $(GSL_LIBS)
xtranslate_SOURCES = xtranslate.cc
