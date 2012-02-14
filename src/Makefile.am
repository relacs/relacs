bin_SCRIPTS = \
    relacs

pkglib_PROGRAMS = \
    relacsgui

dist_pkgdata_DATA = \
    relacstuxheader.png \
    relacssplash.png \
    relacstux.svg \
    RELACS.desktop

EXTRA_DIST = \
    relacslocal_head \
    relacs_head \
    relacs_foot


# TODO
relacsgui_CPPFLAGS = \
    -I$(srcdir)/../daq/include \
    -I$(srcdir)/../datafile/include \
    -I$(srcdir)/../plot/include \
    -I$(srcdir)/../numerics/include \
    -I$(srcdir)/../options/include \
    -I$(srcdir)/../widgets/include \
    -I$(srcdir)/../relacs/include \
    $(QT4_CPPFLAGS)

relacsgui_LDFLAGS = \
    $(QT4_LDFLAGS) \
    $(GSL_LDFLAGS)
# the GSL part should not be here, it is already in librelacs.la!
# but libtool 2.4 seems to require it.

relacsgui_LDADD = \
    ../relacs/src/librelacs.la \
    ../numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) \
    $(GSL_LIBS)
# the GSL and librelacsnumerics part should not be here,
# it is already in librelacs.la!
# but libtool 2.4 seems to require it.

relacsgui_SOURCES = \
    relacsmain.cc


DISTCLEANFILES = relacs.log relacs.log.old
