bin_SCRIPTS = \
    relacs

pkglib_PROGRAMS = \
    relacsgui

sysconf_DATA = \
    relacs.cfg

dist_pkgdata_DATA = \
    relacstuxheader.png



# TODO
relacsgui_CPPFLAGS = \
    "-DRELACSVERSION=\"$(RELACSVERSION)\"" \
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



EXTRA_DIST = \
    relacs_cfg_foot \
    relacs_cfg_head \
    \
    relacs_runner_foot \
    relacs_runner_head


DISTCLEANFILES = relacs.log relacs.log.old
