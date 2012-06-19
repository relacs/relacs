HEADER_PATH = ../include/relacs/camera
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES=

if RELACS_COND_OPENCV
pluginlib_LTLIBRARIES += \
    libcameracameracontrol.la \
    libcameraopencvcameracalibration.la  \
    libcameraopencvstereocameracalibration.la \
    libcameraopencvcalibrationquality.la
endif 


libcameracameracontrol_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS) \
    $(OPENCV_CPPFLAGS)

libcameracameracontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libcameracameracontrol_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS) \
    $(OPENCV_LIBS)

$(libcameracameracontrol_la_OBJECTS) : moc_cameracontrol.cc

libcameracameracontrol_la_SOURCES = cameracontrol.cc

libcameracameracontrol_la_includedir = $(pkgincludedir)/camera

libcameracameracontrol_la_include_HEADERS = $(HEADER_PATH)/cameracontrol.h



libcameraopencvcameracalibration_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)\
    $(OPENCV_CPPFLAGS)


libcameraopencvcameracalibration_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libcameraopencvcameracalibration_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcameraopencvcameracalibration_la_OBJECTS) : moc_opencvcameracalibration.cc

libcameraopencvcameracalibration_la_SOURCES = opencvcameracalibration.cc

libcameraopencvcameracalibration_la_includedir = $(pkgincludedir)/camera

libcameraopencvcameracalibration_la_include_HEADERS = $(HEADER_PATH)/opencvcameracalibration.h


libcameraopencvstereocameracalibration_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)\
    $(OPENCV_CPPFLAGS)


libcameraopencvstereocameracalibration_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libcameraopencvstereocameracalibration_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcameraopencvstereocameracalibration_la_OBJECTS) : moc_opencvstereocameracalibration.cc

libcameraopencvstereocameracalibration_la_SOURCES = opencvstereocameracalibration.cc

libcameraopencvstereocameracalibration_la_includedir = $(pkgincludedir)/camera

libcameraopencvstereocameracalibration_la_include_HEADERS = $(HEADER_PATH)/opencvstereocameracalibration.h



libcameraopencvcalibrationquality_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)\
    $(OPENCV_CPPFLAGS)


libcameraopencvcalibrationquality_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libcameraopencvcalibrationquality_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcameraopencvcalibrationquality_la_OBJECTS) : moc_opencvcalibrationquality.cc

libcameraopencvcalibrationquality_la_SOURCES = opencvcalibrationquality.cc

libcameraopencvcalibrationquality_la_includedir = $(pkgincludedir)/camera

libcameraopencvcalibrationquality_la_include_HEADERS = $(HEADER_PATH)/opencvcalibrationquality.h



check_PROGRAMS = \
    linktest_libcameracameracontrol_la \
    linktest_libcameraopencvcameracalibration_la  \
    linktest_libcameraopencvstereocameracalibration_la \
    linktest_libcameraopencvcalibrationquality_la

linktest_libcameracameracontrol_la_SOURCES = linktest.cc
linktest_libcameracameracontrol_la_LDADD = libcameracameracontrol.la

linktest_libcameraopencvcameracalibration_la_SOURCES = linktest.cc
linktest_libcameraopencvcameracalibration_la_LDADD = libcameraopencvcameracalibration.la

linktest_libcameraopencvstereocameracalibration_la_SOURCES = linktest.cc
linktest_libcameraopencvstereocameracalibration_la_LDADD = libcameraopencvstereocameracalibration.la

linktest_libcameraopencvcalibrationquality_la_SOURCES = linktest.cc
linktest_libcameraopencvcalibrationquality_la_LDADD = libcameraopencvcalibrationquality.la

TESTS = $(check_PROGRAMS)

