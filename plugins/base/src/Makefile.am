HEADER_PATH = ../include/relacs/base
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libbaselinearattenuate.la \
    libbasedecibelattenuate.la \
    libbasehighpass.la \
    libbaselowpass.la \
    libbasespectrumanalyzer.la \
    libbasepause.la \
    libbasesetoutput.la \
    libbasesimplemodel.la \
    libbasetransferfunction.la \
    libbasewait.la \
    libbasecameracontrol.la

if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libbaserobot.la
endif


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



libbasecameracontrol_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(top_srcdir)/relacs/include \
    -I/usr/include/opencv \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libbasecameracontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libbasecameracontrol_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/plugins/linuxdevices/misc/src/libmiscopencvcamera.la \
    $(QT4_LIBS)

$(libbasecameracontrol_la_OBJECTS) : moc_cameracontrol.cc

libbasecameracontrol_la_SOURCES = cameracontrol.cc

libbasecameracontrol_la_includedir = $(pkgincludedir)/base

libbasecameracontrol_la_include_HEADERS = $(HEADER_PATH)/cameracontrol.h



check_PROGRAMS = \
    linktest_libbaselinearattenuate_la \
    linktest_libbasedecibelattenuate_la \
    linktest_libbasehighpass_la \
    linktest_libbaselowpass_la \
    linktest_libbasespectrumanalyzer_la \
    linktest_libbasepause_la \
    linktest_libbasesetoutput_la \
    linktest_libbasesimplemodel_la \
    linktest_libbasetransferfunction_la \
    linktest_libbaserobot_la \
    linktest_libbasewait_la \
    linktest_libbasecameracontrol_la

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

linktest_libbasecameracontrol_la_SOURCES = linktest.cc
linktest_libbasecameracontrol_la_LDADD = libbasecameracontrol.la

TESTS = $(check_PROGRAMS)
