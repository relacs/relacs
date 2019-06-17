bin_SCRIPTS = \
    relacs

# The next line can be removed once we assume automake >= 1.11.
pkglibexecdir = $(libexecdir)/@PACKAGE@

pkglibexec_PROGRAMS = \
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
    -I$(srcdir)/../shapes/include \
    -I$(srcdir)/../daq/include \
    -I$(srcdir)/../datafile/include \
    -I$(srcdir)/../plot/include \
    -I$(srcdir)/../numerics/include \
    -I$(srcdir)/../options/include \
    -I$(srcdir)/../widgets/include \
    -I$(srcdir)/../relacs/include \
    $(QT4_CPPFLAGS) $(NIX_CPPFLAGS)

relacsgui_LDFLAGS = \
    $(QT4_LDFLAGS) \
    $(GSL_LDFLAGS) \
    $(SNDFILE_LDFLAGS) \
    $(PORTAUDIO_LDFLAGS) \
    $(NIX_LDFLAGS)
# the GSL part should not be here, it is already in librelacs.la!
# but libtool 2.4 seems to require it.

relacsgui_LDADD = \
    ../relacs/src/librelacs.la \
    ../numerics/src/librelacsnumerics.la \
    $(QT4_LIBS) \
    $(GSL_LIBS) \
    $(SNDFILE_LIBS) \
    $(PORTAUDIO_LIBS) \
    $(NIX_LIBS)
# the GSL and librelacsnumerics part should not be here,
# it is already in librelacs.la!
# but libtool 2.4 seems to require it.

relacsgui_SOURCES = \
    relacsmain.cc


DISTCLEANFILES = relacs.log relacs.log.old
