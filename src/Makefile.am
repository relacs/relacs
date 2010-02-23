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
    $(QT3_CPPFLAGS)

relacsgui_LDFLAGS = \
    $(QT3_LDFLAGS)

relacsgui_LDADD = \
    ../relacs/src/librelacs.la \
    $(QT3_LIBS)

relacsgui_SOURCES = \
    relacsmain.cc


DISTCLEANFILES = relacs.log relacs.log.old
