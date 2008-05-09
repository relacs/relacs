bin_SCRIPTS = \
    relacs

pkglib_PROGRAMS = \
    relacs_main

sysconf_DATA = \
    relacs.cfg

pkgdata_DATA = \
    relacstuxheader.png



# TODO
relacs_main_CPPFLAGS = \
    "-DRELACSVERSION=\"$(RELACSVERSION)\"" \
    -I$(srcdir)/../daq/include \
    -I$(srcdir)/../datafile/include \
    -I$(srcdir)/../plot/include \
    -I$(srcdir)/../numerics/include \
    -I$(srcdir)/../options/include \
    -I$(srcdir)/../widgets/include \
    -I$(srcdir)/../relacs/include \
    $(QT3_CPPFLAGS)

relacs_main_LDFLAGS = \
    $(QT3_LDFLAGS)

relacs_main_LDADD = \
    ../relacs/src/librelacs.la \
    $(QT3_LIBS)

relacs_main_SOURCES = \
    relacsmain.cc



EXTRA_DIST = \
    relacs_cfg_foot \
    relacs_cfg_head \
    \
    relacs_runner_foot \
    relacs_runner_head


DISTCLEANFILES = relacs.log relacs.log.old
