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


ifeq ($(MAKELEVEL),1)
  TOPDIR = ..
else
  TOPDIR = ../..
endif
include $(TOPDIR)/Makefile.h

SOURCES = tablekey.cc datafile.cc tabledata.cc translate.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
TARGET = libdatafile.so
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $+
	cp -p $@ ../$(LIBRARYPATH)


%.o: %.cc
	$(CC) -c $< $(CFLAGS)
	cp -p $(patsubst %.o,%.h,$@) ../$(INCLUDEPATH)


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(SOURCES)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o *.so *~ *.bak *.tgz


# DEPENDENCIES: (do not delete this line)

tablekey.o: tabledata.h tablekey.h
datafile.o: datafile.h tablekey.h tabledata.h
tabledata.o: tablekey.h tabledata.h
translate.o: translate.h tablekey.h
