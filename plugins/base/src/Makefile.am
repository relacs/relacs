HEADER_PATH = ../include/relacs/base
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libbasetraces.la \
    libbaselinearattenuate.la \
    libbasedecibelattenuate.la \
    libbasehighpass.la \
    libbaselowpass.la \
    libbasespectrumanalyzer.la \
    libbasepause.la \
    libbaserecord.la \
    libbasesetoutput.la \
    libbasesimplemodel.la \
    libbasetransferfunction.la \
    libbasewait.la  \
    libbaseenvelope.la \
    libbasesavetraces.la \
    libbasespectrogram.la


if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libbaserobot.la
endif



libbasetraces_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasetraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasetraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

libbasetraces_la_SOURCES = traces.cc

libbasetraces_la_includedir = $(pkgincludedir)/base

libbasetraces_la_include_HEADERS = $(HEADER_PATH)/traces.h



libbaselinearattenuate_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbaselinearattenuate_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbaselinearattenuate_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

libbaselinearattenuate_la_SOURCES = linearattenuate.cc

libbaselinearattenuate_la_includedir = $(pkgincludedir)/base

libbaselinearattenuate_la_include_HEADERS = $(HEADER_PATH)/linearattenuate.h



libbasedecibelattenuate_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasedecibelattenuate_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasedecibelattenuate_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

libbasedecibelattenuate_la_SOURCES = decibelattenuate.cc

libbasedecibelattenuate_la_includedir = $(pkgincludedir)/base

libbasedecibelattenuate_la_include_HEADERS = $(HEADER_PATH)/decibelattenuate.h



libbasehighpass_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasehighpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasehighpass_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasehighpass_la_OBJECTS) : moc_highpass.cc

libbasehighpass_la_SOURCES = highpass.cc

libbasehighpass_la_includedir = $(pkgincludedir)/base

libbasehighpass_la_include_HEADERS = $(HEADER_PATH)/highpass.h



libbaselowpass_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbaselowpass_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbaselowpass_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbaselowpass_la_OBJECTS) : moc_lowpass.cc

libbaselowpass_la_SOURCES = lowpass.cc

libbaselowpass_la_includedir = $(pkgincludedir)/base

libbaselowpass_la_include_HEADERS = $(HEADER_PATH)/lowpass.h



libbasespectrumanalyzer_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasespectrumanalyzer_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasespectrumanalyzer_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasespectrumanalyzer_la_OBJECTS) : moc_spectrumanalyzer.cc

libbasespectrumanalyzer_la_SOURCES = spectrumanalyzer.cc

libbasespectrumanalyzer_la_includedir = $(pkgincludedir)/base

libbasespectrumanalyzer_la_include_HEADERS = $(HEADER_PATH)/spectrumanalyzer.h



libbasepause_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasepause_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasepause_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasepause_la_OBJECTS) : moc_pause.cc

libbasepause_la_SOURCES = pause.cc

libbasepause_la_includedir = $(pkgincludedir)/base

libbasepause_la_include_HEADERS = $(HEADER_PATH)/pause.h



libbaserecord_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbaserecord_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbaserecord_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbaserecord_la_OBJECTS) : moc_record.cc

libbaserecord_la_SOURCES = record.cc

libbaserecord_la_includedir = $(pkgincludedir)/base

libbaserecord_la_include_HEADERS = $(HEADER_PATH)/record.h



libbasesetoutput_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasesetoutput_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasesetoutput_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasesetoutput_la_OBJECTS) : moc_setoutput.cc

libbasesetoutput_la_SOURCES = setoutput.cc

libbasesetoutput_la_includedir = $(pkgincludedir)/base

libbasesetoutput_la_include_HEADERS = $(HEADER_PATH)/setoutput.h



libbasesimplemodel_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasesimplemodel_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasesimplemodel_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

libbasesimplemodel_la_SOURCES = simplemodel.cc

libbasesimplemodel_la_includedir = $(pkgincludedir)/base

libbasesimplemodel_la_include_HEADERS = $(HEADER_PATH)/simplemodel.h



libbasetransferfunction_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasetransferfunction_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasetransferfunction_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasetransferfunction_la_OBJECTS) : moc_transferfunction.cc

libbasetransferfunction_la_SOURCES = transferfunction.cc

libbasetransferfunction_la_includedir = $(pkgincludedir)/base

libbasetransferfunction_la_include_HEADERS = $(HEADER_PATH)/transferfunction.h



libbaserobot_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbaserobot_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbaserobot_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscmirob.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbaserobot_la_OBJECTS) : moc_robot.cc

libbaserobot_la_SOURCES = robot.cc

libbaserobot_la_includedir = $(pkgincludedir)/base

libbaserobot_la_include_HEADERS = $(HEADER_PATH)/robot.h



libbasewait_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasewait_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasewait_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasewait_la_OBJECTS) : moc_wait.cc

libbasewait_la_SOURCES = wait.cc

libbasewait_la_includedir = $(pkgincludedir)/base

libbasewait_la_include_HEADERS = $(HEADER_PATH)/wait.h



libbaseenvelope_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbaseenvelope_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbaseenvelope_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbaseenvelope_la_OBJECTS) : moc_envelope.cc

libbaseenvelope_la_SOURCES = envelope.cc

libbaseenvelope_la_includedir = $(pkgincludedir)/base

libbaseenvelope_la_include_HEADERS = $(HEADER_PATH)/envelope.h



libbasesavetraces_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasesavetraces_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasesavetraces_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasesavetraces_la_OBJECTS) : moc_savetraces.cc

libbasesavetraces_la_SOURCES = savetraces.cc

libbasesavetraces_la_includedir = $(pkgincludedir)/base

libbasesavetraces_la_include_HEADERS = $(HEADER_PATH)/savetraces.h



libbasespectrogram_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasespectrogram_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasespectrogram_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/datafile/src/librelacsdatafile.la \
    $(top_builddir)/plot/src/librelacsplot.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/widgets/src/librelacswidgets.la \
    $(QT4_LIBS) $(GSL_LIBS)

$(libbasespectrogram_la_OBJECTS) : moc_spectrogram.cc

libbasespectrogram_la_SOURCES = spectrogram.cc

libbasespectrogram_la_includedir = $(pkgincludedir)/base

libbasespectrogram_la_include_HEADERS = $(HEADER_PATH)/spectrogram.h



check_PROGRAMS = \
    linktest_libbaselinearattenuate_la \
    linktest_libbasedecibelattenuate_la \
    linktest_libbasehighpass_la \
    linktest_libbaselowpass_la \
    linktest_libbasespectrumanalyzer_la \
    linktest_libbasepause_la \
    linktest_libbaserecord_la \
    linktest_libbasesetoutput_la \
    linktest_libbasesimplemodel_la \
    linktest_libbasetransferfunction_la \
    linktest_libbasewait_la  \
    linktest_libbaseenvelope_la \
    linktest_libbasesavetraces_la \
    linktest_libbasespectrogram_la

if RELACS_COND_TML
check_PROGRAMS += \
    linktest_libbaserobot_la
endif


linktest_libbaselinearattenuate_la_SOURCES = linktest.cc
linktest_libbaselinearattenuate_la_LDADD = libbaselinearattenuate.la

linktest_libbasedecibelattenuate_la_SOURCES = linktest.cc
linktest_libbasedecibelattenuate_la_LDADD = libbasedecibelattenuate.la

linktest_libbasehighpass_la_SOURCES = linktest.cc
linktest_libbasehighpass_la_LDADD = libbasehighpass.la

linktest_libbaselowpass_la_SOURCES = linktest.cc
linktest_libbaselowpass_la_LDADD = libbaselowpass.la

linktest_libbasespectrumanalyzer_la_SOURCES = linktest.cc
linktest_libbasespectrumanalyzer_la_LDADD = libbasespectrumanalyzer.la

linktest_libbasepause_la_SOURCES = linktest.cc
linktest_libbasepause_la_LDADD = libbasepause.la

linktest_libbaserecord_la_SOURCES = linktest.cc
linktest_libbaserecord_la_LDADD = libbaserecord.la

linktest_libbasesetoutput_la_SOURCES = linktest.cc
linktest_libbasesetoutput_la_LDADD = libbasesetoutput.la

linktest_libbasesimplemodel_la_SOURCES = linktest.cc
linktest_libbasesimplemodel_la_LDADD = libbasesimplemodel.la

linktest_libbasetransferfunction_la_SOURCES = linktest.cc
linktest_libbasetransferfunction_la_LDADD = libbasetransferfunction.la

linktest_libbaserobot_la_SOURCES = linktest.cc
linktest_libbaserobot_la_LDADD = libbaserobot.la

linktest_libbasewait_la_SOURCES = linktest.cc
linktest_libbasewait_la_LDADD = libbasewait.la


linktest_libbaseenvelope_la_SOURCES = linktest.cc
linktest_libbaseenvelope_la_LDADD = libbaseenvelope.la

linktest_libbasesavetraces_la_SOURCES = linktest.cc
linktest_libbasesavetraces_la_LDADD = libbasesavetraces.la

linktest_libbasespectrogram_la_SOURCES = linktest.cc
linktest_libbasespectrogram_la_LDADD = libbasespectrogram.la

TESTS = $(check_PROGRAMS)
