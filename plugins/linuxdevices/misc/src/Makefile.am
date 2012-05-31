HEADER_PATH = ../include/relacs/misc

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libmisckleindiek.la \
    libmisctempdtm5080.la \
    libmiscamplmode.la \
    libmiscopencvcamera.la
if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libmiscmirob.la
endif 



libmiscamplmode_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../../nieseries/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libmiscamplmode_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libmiscamplmode_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

libmiscamplmode_la_SOURCES = amplmode.cc amplmodedevice.cc

libmiscamplmode_la_includedir = $(pkgincludedir)/misc

libmiscamplmode_la_include_HEADERS = $(HEADER_PATH)/amplmode.h



libmiscmirob_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(TML_CPPFLAGS) \
    $(QT4_CPPFLAGS)

libmiscmirob_la_LDFLAGS = \
    -module -avoid-version \
    $(TML_LDFLAGS) \
    $(QT4_LDFLAGS)

libmiscmirob_la_LIBADD = \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(TML_LIBS) \
    $(QT4_LIBS)

libmiscmirob_la_SOURCES = mirob.cc mirobdevice.cc

libmiscmirob_la_includedir = $(pkgincludedir)/misc

libmiscmirob_la_include_HEADERS = $(HEADER_PATH)/mirob.h




libmiscopencvcamera_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    -I/usr/include/opencv \
    $(QT4_CPPFLAGS)

libmiscopencvcamera_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libmiscopencvcamera_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT4_LIBS) \
    -lcv \
    -lhighgui \
    -lcvaux


libmiscopencvcamera_la_SOURCES = opencvcamera.cc opencvcameradevice.cc

libmiscopencvcamera_la_includedir = $(pkgincludedir)/misc

libmiscopencvcamera_la_include_HEADERS = $(HEADER_PATH)/opencvcamera.h  




libmisckleindiek_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libmisckleindiek_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libmisckleindiek_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

libmisckleindiek_la_SOURCES = kleindiek.cc kleindiekdevice.cc

libmisckleindiek_la_includedir = $(pkgincludedir)/misc

libmisckleindiek_la_include_HEADERS = $(HEADER_PATH)/kleindiek.h



libmisctempdtm5080_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT4_CPPFLAGS)

libmisctempdtm5080_la_LDFLAGS = \
    -module -avoid-version \
    $(QT4_LDFLAGS)

libmisctempdtm5080_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) $(GSL_LIBS)

libmisctempdtm5080_la_SOURCES = tempdtm5080.cc tempdtm5080device.cc

libmisctempdtm5080_la_includedir = $(pkgincludedir)/misc

libmisctempdtm5080_la_include_HEADERS = $(HEADER_PATH)/tempdtm5080.h






check_PROGRAMS = \
    linktest_libmiscamplmode_la \
    linktest_libmisckleindiek_la \
    linktest_libmisctempdtm5080_la
if RELACS_COND_TML
check_PROGRAMS += \
    linktest_libmiscmirob_la
endif 

linktest_libmiscamplmode_la_SOURCES = linktest.cc
linktest_libmiscamplmode_la_LDADD = libmiscamplmode.la

linktest_libmiscmirob_la_SOURCES = linktest.cc
linktest_libmiscmirob_la_LDADD = libmiscmirob.la



linktest_libmisckleindiek_la_SOURCES = linktest.cc
linktest_libmisckleindiek_la_LDADD = libmisckleindiek.la

linktest_libmisctempdtm5080_la_SOURCES = linktest.cc
linktest_libmisctempdtm5080_la_LDADD = libmisctempdtm5080.la


TESTS = $(check_PROGRAMS)
