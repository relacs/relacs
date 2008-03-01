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


TOPDIR = ..

include $(TOPDIR)/Makefile.h

SOURCES = niai.cc niao.cc nidio.cc nipfi.cc nieserieshardware.cc \
	  comedianaloginput.cc comedianalogoutput.cc comedihardware.cc \
	  cs3310nidio.cc cs3310serialnidio.cc cs3310pp.cc cs3310hardware.cc \
	  amplmode.cc \
	  kleindiek.cc \
	  tempdtm5080.cc \
	  hardware.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
HEADERS = nidaq.h
TARGET = libhardware.so libnieserieshardware.so # libcomedihardware.so
BACKUPFILES = Makefile loadcomedimodules.sh killcomedimodules.sh

# path to the plugin libraries:
PLUGINLIBPATH = plugins
# path to the plugin header files:
PLUGININCPATH = include
# include pathes for the plugins:
PLUGININCLUDES = -Linclude
# library pathes for the plugins:
PLUGINLIBRARIES = -Lplugins


DEFS     = -DNIDAQ_MAJOR=254


ORGRELACSINC = -I../include -I.


.PHONY: all depend backupfiles examples doc help clean


all: $(TARGET)


libhardware.so: cs3310pp.o kleindiek.o tempdtm5080.o

libnieserieshardware.so: niai.o niao.o nidio.o nipfi.o \
          cs3310nidio.o cs3310serialnidio.o amplmode.o

libcomedihardware.so: comedianaloginput.o comedianalogoutput.o


lib%.so: %.o
	$(CC) -shared -o $@ $+
	cp -p $@ tmp.so; mv tmp.so $(PLUGINLIBPATH)/$@;
	cp -p *.h $(PLUGININCPATH)

%.o: %.cc
	$(CC) -c $< $(CFLAGS) $(DEFS) $(ORGRELACSINC) $(QTINC)


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) $(ORGRELACSINC) -- $(SOURCES)
	$(MAKE) -e -C examples depend


backupfiles:
	rm -f $(BACKUPFILESFILE).tmp
	for dir in examples; \
	  do $(MAKE) -e -C $$dir backupfiles; \
	  for i in $$(<$(TOPDIR)/$(BACKUPFILESFILE)); \
	    do echo $$dir/$$i; \
	  done >> $(BACKUPFILESFILE).tmp; \
	done
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	  do echo $$i; \
	done >> $(BACKUPFILESFILE).tmp;
	mv $(BACKUPFILESFILE).tmp $(TOPDIR)/$(BACKUPFILESFILE)


examples:
	$(MAKE) -e -C examples

doc:

help:

clean:
	rm -f *.o *.so *~ *.bak plugins/* include/*


# DEPENDENCIES: (do not delete this line)

niai.o: niao.h nidaq.h niai.h
niao.o: niai.h nidaq.h niao.h
nidio.o: nidaq.h nidio.h
nipfi.o: nidaq.h nipfi.h
nieserieshardware.o: niai.h nidaq.h niao.h nidio.h nipfi.h
nieserieshardware.o: cs3310serialnidio.h cs3310nidio.h amplmode.h
comedianaloginput.o: comedianalogoutput.h comedianaloginput.h
comedianalogoutput.o: comedianaloginput.h comedianalogoutput.h
comedihardware.o: comedianaloginput.h comedianalogoutput.h
cs3310nidio.o: cs3310nidio.h nidio.h
cs3310serialnidio.o: cs3310serialnidio.h nidio.h
cs3310pp.o: cs3310pp.h
amplmode.o: amplmode.h nidio.h
kleindiek.o: kleindiek.h
tempdtm5080.o: tempdtm5080.h
hardware.o: cs3310pp.h kleindiek.h tempdtm5080.h
