noinst_PROGRAMS = \
    checkao


AM_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(srcdir)/../include

AM_LDFLAGS = \
    $(QT3_LDFLAGS)

checkao_LDADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    ../src/libcomedicomedianaolginput.la \
    ../src/libcomedicomedianaolgoutput.la
checkao_SOURCES = checkao.cc
