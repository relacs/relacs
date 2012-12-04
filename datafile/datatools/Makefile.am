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
    -I$(srcdir)/../../options/include \
    -I$(srcdir)/../include \
    $(GSL_CPPFLAGS) \
    $(SNDFILE_CPPFLAGS)

AM_LDFLAGS = \
    $(GSL_LDFLAGS) \
    $(SNDFILE_LDFLAGS)

bindata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
bindata_SOURCES = bindata.cc


convertdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
convertdata_SOURCES = convertdata.cc


datacolumn_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
datacolumn_SOURCES = datacolumn.cc


datainfo_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
datainfo_SOURCES = datainfo.cc


datastats_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
datastats_SOURCES = datastats.cc


extractdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
extractdata_SOURCES = extractdata.cc


fixdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
fixdata_SOURCES = fixdata.cc


mergedata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
mergedata_SOURCES = mergedata.cc


plotdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
plotdata_SOURCES = plotdata.cc


selectdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
selectdata_SOURCES = selectdata.cc


splitdata_LDADD = \
    ../../numerics/src/librelacsnumerics.la \
    ../../options/src/librelacsoptions.la \
    ../src/librelacsdatafile.la \
    $(GSL_LIBS) $(SNDFILE_LIBS)
splitdata_SOURCES = splitdata.cc


