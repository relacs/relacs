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


ifeq ($(MAKELEVEL),0)
  TOPDIR = .
else
  TOPDIR = ..
endif

include Makefile.h

# destination for the header files and libraries:
INCLUDEPATH = include
LIBRARYPATH = lib

# installation destination for the header files:
INSTALLINCLUDEPATH = $(HOME)/include
# installation destination for the library:
INSTALLLIBRARYPATH = $(HOME)/lib

export INCLUDEPATH LIBRARYPATH

BACKUPFILES = Makefile Makefile.h


.PHONY: all src examples doc depend install backup backupfiles clean


src:
	$(MAKE) -e -C src


examples:
	$(MAKE) -e -C examples


all: src examples


doc:
	doxygen doc/numerics.dox

depend:
	$(MAKE) -C src depend
	$(MAKE) -C examples depend


install: src
	cp -p $(INCLUDEPATH)/*.h $(INSTALLINCLUDEPATH)
	cp -p $(LIBRARYPATH)/*.so $(INSTALLLIBRARYPATH)


backup:
	tar czvf numerics.tgz Makefile Makefile.h src/*[!~o] examples/*[!~o]

backupfiles:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in src examples; \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(TOPDIR)/$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	for i in $(BACKUPFILES); \
	  do echo $$i; \
	done >> $(BACKUPFILESFILE).tmp;
	mv $(BACKUPFILESFILE).tmp $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	$(MAKE) -C src clean
	rm -f include/* lib/* *.tgz *~ *.bak
