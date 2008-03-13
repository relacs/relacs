###############################################################################
#
# Makefile
# 
#
# RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
# Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# RELACS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

export RELEASE =

include ./Makefile.h

# directories containing library source files needed for RELACS:
LIBDIRS = numerics daq options datafile mplot widgets config
# directories containing RELACS source files:
RELACSDIRS = src
# directories and descriptions of plugin sets:
PLUGINSETS = hardware:Hardware common:Common auditory:Auditory patch:Patch-Clamp
# directories containing plugins (RePros, Detectors, Controls):
PLUGINSDIRS = hardware common auditory patch
# additional files/directories for documentation:
DOCDIRS = daq/src options/src datafile/src \
          mplot/src widgets/src config/src \
          doc utils
# additional directories to LIBDIRS, RELACSDIRS, and PLUGINSDIRS 
# that should be backupped:
BACKUPDIRS = utils examples
# single files besides the ones in LIBDIRS, RELACSDIRS, PLUGINSDIRS, BACKUPSRCDIRS,
# and BACKUPDIRS which should be included in a backup:
BACKUPFILES = Makefile \
	      README INSTALL COPYING AUTHORS THANKS NEWS ChangeLog TODO \
	      doc/relacs.1 \
	      doc/relacs.dox doc/*.doc doc/*.fig doc/*.png
# path to the RELACS web page that is backupped with 'make backupallweb'
# and 'make release':
BACKUPWEB = doc/web logo
# single plugins files besides the ones in PLUGINSDIRS
# which should be included in a plugins backup:
BACKUPPLUGINSFILES = Makefile.h
# additional files that should be included in a release:
RELEASEFILES = *.cfg

# A file that is used to store the names of all files 
# that should be stored in the archive:
export BACKUPFILESFILE = backup.fls


# directories which are needed in addition to the ones saved in a backup:
DIRSKEL = include lib \
          backups dry \
          numerics/include numerics/lib \
          daq/include daq/lib \
          options/include options/lib \
          datafile/include datafile/lib \
	  mplot/include mplot/lib \
          widgets/include widgets/lib \
          config/include config/lib \
          hardware/plugins hardware/include \
          common/plugins common/include \
          auditory/plugins auditory/include \
          patch/plugins patch/include


.PHONY: relacsonly all plugins \
	install installlibs installnumerics installplugins installall \
        backup backupplugins backupall \
	release \
	web doc docall docplugins help \
	depend dependplugins dependall \
	clean cleanplugins cleanall \
        $(LIBDIRS) $(RELACSDIRS) $(PLUGINSDIRS)

# default make:
relacsonly: $(LIBDIRS) $(RELACSDIRS) relacsexec relacs

plugins: $(PLUGINSDIRS)

all: relacsonly plugins


$(LIBDIRS):
	$(MAKE) -e -C $@
	cp -p $@/lib/*.so lib
	cp -p $@/include/*.h include


$(RELACSDIRS):
	$(MAKE) -e -C $@


$(PLUGINSDIRS):
	$(MAKE) -e -C $@


relacs: relacsexec utils/relacsscript
	PP=""; for dir in $(PLUGINSDIRS); do PP=$$PP:$(CURDIR)/$$dir/plugins; done; \
	sed "s|RELACSEXECPATH=\.|RELACSEXECPATH=$(CURDIR)|; s|RELACSPLUGINPATHES=./plugins|RELACSPLUGINPATHES=$${PP#:}|;" utils/relacsscript > relacs
	chmod a+x relacs


relacsexec: lib/libnumerics.so lib/libdaq.so lib/liboptions.so \
	lib/libdatafile.so lib/libmplot.so lib/libwidgets.so \
	lib/libconfig.so lib/librelacs.so
	cp $+ .
	$(CC) -rdynamic -o $@ $(patsubst lib/%,%,$+) $(DLLIB) $(QTLIB)
	rm lib*.so
#	$(CC) -o $@ $+ $(QTLIB)


install:
	cp $(RELACSLIB)/lib*.so $(LIBINSTALLPATH)
	cp relacsexec $(RELACSINSTALLPATH)
	cp relacs $(RELACSINSTALLPATH)

installlibs:
	$(MAKE) -e -C numerics install
	$(MAKE) -e -C daq install
	$(MAKE) -e -C options install
	$(MAKE) -e -C datafile install
	$(MAKE) -e -C datafile/datatools install

installnumerics:
	$(MAKE) -e -C numerics install

installplugins:
	cp $(RELACSPLUGINS)/lib*.so $(PLUGINSINSTALLPATH)
	cp $(RELACSPLUGINSLIB)/lib*.so $(PLUGINSLIBINSTALLPATH)

installall: install installplugins


backup:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(LIBDIRS) $(RELACSDIRS) $(BACKUPDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	mv $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)
	tar czv $(foreach name,$(DIRSKEL),--exclude=$(name)/*) \
	-f backups/relacs-$(VERSION).tgz -T $(BACKUPFILESFILE) \
	$(DIRSKEL) $(BACKUPFILES) || true
	rm -f relacs.tgz
	cp backups/relacs-$(VERSION).tgz relacs.tgz
	rm -f $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)

backupplugins:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	mv $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)
	tar czvf backups/relacsplugins-$(VERSION).tgz \
	-T $(BACKUPFILESFILE) $(BACKUPPLUGINSFILES) || true
	rm -f relacsplugins.tgz
	cp backups/relacsplugins-$(VERSION).tgz relacsplugins.tgz
	rm -f $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)

backupall:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(LIBDIRS) $(RELACSDIRS) $(BACKUPDIRS) $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	mv $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)
	tar czv $(foreach name,$(DIRSKEL),--exclude=$(name)/*) \
	-f backups/relacs-$(VERSION).tgz -T $(BACKUPFILESFILE) \
	$(DIRSKEL) $(BACKUPFILES) $(BACKUPPLUGINSFILES) || true
	rm -f relacs.tgz
	cp backups/relacs-$(VERSION).tgz relacs.tgz
	rm -f $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)

backupallweb:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(LIBDIRS) $(RELACSDIRS) $(BACKUPDIRS) $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	mv $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)
	tar czv $(foreach name,$(DIRSKEL),--exclude=$(name)/*) \
	-f backups/relacs-$(VERSION).tgz -T $(BACKUPFILESFILE) \
	$(DIRSKEL) $(BACKUPFILES) $(BACKUPWEB) $(BACKUPPLUGINSFILES) || true
	rm -f relacs.tgz
	cp backups/relacs-$(VERSION).tgz relacs.tgz
	rm -f $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)

release:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(LIBDIRS) $(RELACSDIRS) $(BACKUPDIRS) $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	mv $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)
	rm -f relacs.tgz
	tar czv $(foreach name,$(DIRSKEL),--exclude=$(name)/*) \
	-f relacs.tgz -T $(BACKUPFILESFILE) \
	$(DIRSKEL) $(BACKUPFILES) $(BACKUPWEB) $(BACKUPPLUGINSFILES) $(RELEASEFILES) || true
	rm -f $(BACKUPFILESFILE).tmp $(BACKUPFILESFILE)


web:
	utils/createweb $(PLUGINSETS)


doc:
	sed "1,20s-version.*-version $(VERSION)-; 1,20s-date.*-date $(DATE)-;" doc/relacs.doc > tmp.doc
	mv tmp.doc doc/relacs.doc
	sed "s-^INPUT.*-INPUT = $(DOCDIRS) $(RELACSDIRS)-; s-^PREDEFINED.*-PREDEFINED = RELACSDATA $(GSL) $(RAND55)-" doc/relacs.dox > tmp.dox
	doxygen tmp.dox
	rm -f tmp.dox

docall:
	sed "1,20s-\\version.*-\\version $(VERSION)-; 1,20s-\\date.*-\\date $(DATE)-;" doc/relacs.doc > tmp.doc
	mv tmp.doc doc/relacs.doc
	sed "s-^INPUT .*-INPUT = $(DOCDIRS) $(RELACSDIRS) $(PLUGINSDIRS)-; s-^PREDEFINED.*-PREDEFINED = RELACSDATA $(GSL) $(RAND55)-" doc/relacs.dox > tmp.dox
	doxygen tmp.dox
	rm -f tmp.dox

docplugins:
	for dir in $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir doc; \
	done

help:
	for dir in $(PLUGINSDIRS); \
	  do $(MAKE) -e -C $$dir help; \
	done


depend:
	for dir in $(LIBDIRS) $(RELACSDIRS); do $(MAKE) -e -C $$dir depend; done

dependplugins:
	for dir in $(PLUGINSDIRS); do $(MAKE) -e -C $$dir depend; done

dependall: depend dependplugins


clean:
	rm -f relacs
	rm -f relacsexec
	rm -f include/*.*
	rm -f dry/*.*
	rm -f lib/*.*
	rm -f *~
	for dir in $(LIBDIRS) $(RELACSDIRS); do $(MAKE) -e -C $$dir clean; done

cleanplugins:
	rm -f plugins/*.*
	rm -f plugins/include/*.*
	for dir in $(PLUGINSDIRS); do $(MAKE) -e -C $$dir clean; done

cleanall: clean cleanplugins
