bin_SCRIPTS = \
    relacs

pkglib_PROGRAMS = \
    relacs_main
    
sysconf_DATA = \
    relacs.cfg



# TODO
relacs_main_CPPFLAGS = \
    "-DRELACSVERSION=\"@RELACSVERSION@\"" \
    -I../config/include \
    -I../daq/include \
    -I../datafile/include \
    -I../mplot/include \
    -I../numerics/include \
    -I../options/include \
    -I../widgets/include \
    -I../relacs/include \
    @QT3_CPPFLAGS@

relacs_main_LDADD = \
    ../relacs/src/librelacs.la

relacs_main_SOURCES = \
    relacsmain.cc

