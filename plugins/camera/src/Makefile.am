HEADER_PATH = ../include/relacs/camera
include ${top_srcdir}/moc4.mk

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libcameracameracontrol.la


libcameracameracontrol_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/datafile/include \
    -I$(top_srcdir)/plot/include \
    -I$(top_srcdir)/widgets/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/plugins/linuxdevices/misc/include \
    -I/usr/include/opencv \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libcameracameracontrol_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libcameracameracontrol_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS)

$(libcameracameracontrol_la_OBJECTS) : moc_cameracontrol.cc

libcameracameracontrol_la_SOURCES = cameracontrol.cc

libcameracameracontrol_la_includedir = $(pkgincludedir)/camera

libcameracameracontrol_la_include_HEADERS = $(HEADER_PATH)/cameracontrol.h



check_PROGRAMS = \
    linktest_libcameracameracontrol_la

linktest_libcameracameracontrol_la_SOURCES = linktest.cc
linktest_libcameracameracontrol_la_LDADD = libcameracameracontrol.la

TESTS = $(check_PROGRAMS)

