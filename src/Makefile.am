#noinst_PROGRAMS = \
#    ../relacs_local

bin_SCRIPTS = \
    relacs

pkglib_PROGRAMS = \
    relacs_main
    
sysconf_DATA = \
    relacs.cfg

#    relacs \#



PLUGINSDIRS = auditory common hardware patch

# TODO

#../relacs_local: relacs_main ../utils/relacsscript Makefile.am
#	PP=""; for dir in $(PLUGINSDIRS); do PP="$$PP:$(CURDIR)/../$$dir/.libs"; done; \
#		sed "s|RELACSEXECPATH=\.|RELACSEXECPATH=$(CURDIR)|; s|RELACSPLUGINPATHES=./plugins|RELACSPLUGINPATHES=$${PP#:}|;" ../utils/relacsscript > ../relacs_local
#	chmod a+x ../relacs_local

#relacs: relacs_main ../utils/relacsscript Makefile.am
#	PP=""; for dir in $(PLUGINSDIRS); do PP="$$PP:$(CURDIR)/../$$dir/.libs"; done; \
#		sed "s|RELACSEXECPATH=\.|RELACSEXECPATH=$(CURDIR)|; s|RELACSPLUGINPATHES=./plugins|RELACSPLUGINPATHES=$${PP#:}|;" ../utils/relacsscript > relacs
#	chmod a+x relacs



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

