###############################################################################
#
# Makefile
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


TOPDIR = ..

include $(TOPDIR)/Makefile.h

# subdirectories containing plugins:
SUBDIRS = base
# some additional files to be backed up:
BACKUPFILES = Makefile doc/*.dox doc/*.png doc/*.jpg doc/*.gif


# compile plugins in all subdirectories:
.PHONY: all backup backupfiles doc help install depend clean $(SUBDIRS)
all: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -e -C $@


# Backup the plugin source files in a tgz-archive:
backup: BF = $$(find . -name "Makefile" -or -name "*.h" -or -name "*.cc")
backup:
	tar czvf patch.tgz $(BF)

# Create a list of files to be backed up in $(TOPDIR)/$(BACKUPFILESFILE):
backupfiles:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in $(SUBDIRS); \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(TOPDIR)/$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	for i in $(BACKUPFILES); \
	  do [ -f $$i ] && echo $$i || true; \
	done >> $(BACKUPFILESFILE).tmp;
	mv $(BACKUPFILESFILE).tmp $(TOPDIR)/$(BACKUPFILESFILE)


# Create plugin documentation:
doc:
	sed "s-^INPUT.*-INPUT = $(SUBDIRS)-;" doc/patch.dox > tmp.dox
	doxygen tmp.dox
	rm -f tmp.dox

# Create RePro help files:
help: doc
	mkdir -p help
	$(TOPDIR)/utils/createhelp help


# Copy the libraries into the plugins directory: 
install:
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir install; done


# Generate dependencies:
depend:
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir depend; done


# Remove all object files, libraries, and backup files.
# Only leave the source files of the plugins:
clean:
	rm -f plugins/* include/*
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir clean; done
