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
    ../src/librelacsdatafile.la
copydata_SOURCES = copydata.cc

pipe_CPPFLAGS = 
pipe_LDADD = 
pipe_SOURCES = pipe.cc

processdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la
processdata_SOURCES = processdata.cc

xdatafile_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la
xdatafile_SOURCES = xdatafile.cc

xtablekey_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la
xtablekey_SOURCES = xtablekey.cc

xtranslate_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la
xtranslate_SOURCES = xtranslate.cc
