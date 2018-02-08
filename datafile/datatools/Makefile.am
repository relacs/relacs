bin_PROGRAMS = \
    bindata \
    convertdata \
    datacolumn \
    datainfo \
    datastats \
    extractdata \
    fixdata \
    mergedata \
    plotdata \
    selectdata \
    splitdata


AM_CPPFLAGS = \
    -I$(srcdir)/../../numerics/include \
    -I$(srcdir)/../../daq/include \
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(GSL_CPPFLAGS) \
    $(SNDFILE_CPPFLAGS)

AM_LDFLAGS = \
    $(GSL_LDFLAGS) \
    $(SNDFILE_LDFLAGS)

LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../../daq/src/librelacsdaq.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)

bindata_SOURCES = bindata.cc

convertdata_SOURCES = convertdata.cc

datacolumn_SOURCES = datacolumn.cc

datainfo_SOURCES = datainfo.cc

datastats_SOURCES = datastats.cc

extractdata_SOURCES = extractdata.cc

fixdata_SOURCES = fixdata.cc

mergedata_SOURCES = mergedata.cc

plotdata_SOURCES = plotdata.cc

selectdata_SOURCES = selectdata.cc

splitdata_SOURCES = splitdata.cc


