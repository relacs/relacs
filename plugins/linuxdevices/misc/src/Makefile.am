HEADER_PATH = ../include/relacs/misc

pluginlibdir = $(pkglibdir)/plugins

pluginlib_LTLIBRARIES = \
    libmisckleindiek.la \
    libmisctempdtm5080.la \
    libmiscamplmode.la
if RELACS_COND_TML
pluginlib_LTLIBRARIES += \
    libmiscmirob.la \
    libmiscxyzrobot.la
endif 
if RELACS_COND_OPENCV
pluginlib_LTLIBRARIES += \
    libmiscopencvcamera.la \
    libmiscopencvstereocamera.la
endif 



libmiscamplmode_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../../nieseries/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmiscamplmode_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmiscamplmode_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)


libmiscamplmode_la_SOURCES = amplmode.cc amplmodedevice.cc

libmiscamplmode_la_includedir = $(pkgincludedir)/misc

libmiscamplmode_la_include_HEADERS = $(HEADER_PATH)/amplmode.h



libmiscmirob_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(TML_CPPFLAGS) \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmiscmirob_la_LDFLAGS = \
    -module -avoid-version \
    $(TML_LDFLAGS) \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmiscmirob_la_LIBADD = \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(TML_LIBS) \
    $(QT_LIBS) $(NIX_LIBS)

libmiscmirob_la_SOURCES = mirob.cc mirobdevice.cc

libmiscmirob_la_includedir = $(pkgincludedir)/misc

libmiscmirob_la_include_HEADERS = $(HEADER_PATH)/mirob.h



libmiscxyzrobot_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(TML_CPPFLAGS) \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmiscxyzrobot_la_LDFLAGS = \
    -module -avoid-version \
    $(TML_LDFLAGS) \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmiscxyzrobot_la_LIBADD = \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/relacs/src/librelacs.la \
    libmiscmirob.la \
    $(TML_LIBS) \
    $(QT_LIBS) $(NIX_LIBS)

libmiscxyzrobot_la_SOURCES = xyzrobot.cc xyzrobotdevice.cc

libmiscxyzrobot_la_includedir = $(pkgincludedir)/misc

libmiscxyzrobot_la_include_HEADERS = $(HEADER_PATH)/xyzrobot.h



libmiscopencvcamera_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS) \
    $(OPENCV_CPPFLAGS)

libmiscopencvcamera_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libmiscopencvcamera_la_LIBADD = \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/relacs/src/librelacs.la \
    $(QT_LIBS) $(NIX_LIBS) \
    $(OPENCV_LIBS) \
    $(GSL_LIBS) \
    -lpthread

libmiscopencvcamera_la_SOURCES = opencvcamera.cc opencvcameradevice.cc

libmiscopencvcamera_la_includedir = $(pkgincludedir)/misc

libmiscopencvcamera_la_include_HEADERS = $(HEADER_PATH)/opencvcamera.h  


libmiscopencvstereocamera_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS) \
    $(OPENCV_CPPFLAGS)

libmiscopencvstereocamera_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS) \
    $(OPENCV_LDFLAGS)

libmiscopencvstereocamera_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsdshapes.la \
    $(QT_LIBS) $(NIX_LIBS) \
    $(OPENCV_LIBS) \
    $(GSL_LIBS)

libmiscopencvstereocamera_la_SOURCES = opencvstereocamera.cc opencvstereocameradevice.cc

libmiscopencvstereocamera_la_includedir = $(pkgincludedir)/misc

libmiscopencvstereocamera_la_include_HEADERS = $(HEADER_PATH)/opencvstereocamera.h  



libmisckleindiek_la_CPPFLAGS = \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmisckleindiek_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmisckleindiek_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libmisckleindiek_la_SOURCES = kleindiek.cc kleindiekdevice.cc

libmisckleindiek_la_includedir = $(pkgincludedir)/misc

libmisckleindiek_la_include_HEADERS = $(HEADER_PATH)/kleindiek.h



libmisctempdtm5080_la_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/shapes/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(top_srcdir)/relacs/include \
    -I$(top_srcdir)/widgets/include \
    -I$(srcdir)/../include \
    $(QT_CPPFLAGS) $(NIX_CPPFLAGS)

libmisctempdtm5080_la_LDFLAGS = \
    -module -avoid-version \
    $(QT_LDFLAGS) $(NIX_LDFLAGS)

libmisctempdtm5080_la_LIBADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    $(top_builddir)/shapes/src/librelacsshapes.la \
    $(top_builddir)/numerics/src/librelacsnumerics.la \
    $(QT_LIBS) $(NIX_LIBS) $(GSL_LIBS)

libmisctempdtm5080_la_SOURCES = tempdtm5080.cc tempdtm5080device.cc

libmisctempdtm5080_la_includedir = $(pkgincludedir)/misc

libmisctempdtm5080_la_include_HEADERS = $(HEADER_PATH)/tempdtm5080.h



check_PROGRAMS = \
    linktest_libmiscamplmode_la \
    linktest_libmisckleindiek_la \
    linktest_libmisctempdtm5080_la
if RELACS_COND_TML
check_PROGRAMS += \
    linktest_libmiscmirob_la \
    linktest_libmiscxyzrobot_la
endif 
if RELACS_COND_OPENCV
check_PROGRAMS += \
    linktest_libmiscopencvcamera_la \
    linktest_libmiscopencvstereocamera_la
endif 

linktest_libmiscamplmode_la_SOURCES = linktest.cc
linktest_libmiscamplmode_la_LDADD = libmiscamplmode.la

linktest_libmiscmirob_la_SOURCES = linktest.cc
linktest_libmiscmirob_la_LDADD = libmiscmirob.la

linktest_libmiscxyzrobot_la_SOURCES = linktest.cc
linktest_libmiscxyzrobot_la_LDADD = libmiscxyzrobot.la

linktest_libmisckleindiek_la_SOURCES = linktest.cc
linktest_libmisckleindiek_la_LDADD = libmisckleindiek.la

linktest_libmisctempdtm5080_la_SOURCES = linktest.cc
linktest_libmisctempdtm5080_la_LDADD = libmisctempdtm5080.la

linktest_libmiscopencvcamera_la_SOURCES = linktest.cc
linktest_libmiscopencvcamera_la_LDADD = libmiscopencvcamera.la

linktest_libmiscopencvstereocamera_la_SOURCES = linktest.cc
linktest_libmiscopencvstereocamera_la_LDADD = libmiscopencvstereocamera.la


TESTS = $(check_PROGRAMS)
