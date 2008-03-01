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
  TOPDIR = ..
else
  ifeq ($(MAKELEVEL),1)
    TOPDIR = ..
  else
    TOPDIR = ../..
  endif
endif
include $(TOPDIR)/Makefile.h

SOURCES = configclass.cc configdialog.cc configure.cc

SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
HEADERS = 
MSOURCES = mconfigdialog.cc
MOBJECTS = $(MSOURCES:.cc=.o)
TARGET = libconfig.so
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGET)


$(TARGET): $(OBJECTS) $(MOBJECTS)
	$(CC) -shared -o $@ $+
	cp -p $@ ../$(LIBRARYPATH)


$(OBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(QTINC)
	cp -p $(patsubst %.o,%.h,$@) ../$(INCLUDEPATH)

$(MOBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(QTINC)

$(MSOURCES): m%.cc: %.h
	$(MOC) $< -o $@


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(OBJECTS:.o=.cc)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o *.so $(MSOURCES) $(TARGET) *~ *.bak


# DEPENDENCIES: (do not delete this line)

configclass.o: configure.h configclass.h
configdialog.o: configdialog.h configclass.h
configure.o: configure.h
