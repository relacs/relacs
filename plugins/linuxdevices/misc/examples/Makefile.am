noinst_PROGRAMS = \
    mm3a \
    temp


AM_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(top_srcdir)/options/include \
    -I$(srcdir)/../include

AM_LDFLAGS = \
    $(QT3_LDFLAGS)

mm3a_LDADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    ../src/libmisckleindiek.la
mm3a_SOURCES = mm3a.cc

temp_LDADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    $(top_builddir)/daq/src/librelacsdaq.la \
    $(top_builddir)/options/src/librelacsoptions.la \
    ../src/libmisctempdtm5080.la
temp_SOURCES = temp.cc
