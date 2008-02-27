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

CFLAGS += -I../include
LIBRARIES = -L../lib -loptions

TARGETS = xtablekey \
          xdatafile copydata processdata pipe
SOURCES = $(foreach name,$(TARGETS),$(name).cc)
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGETS)


$(TARGETS): %: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBRARIES) -lm

%.o: %.cc
	$(CC) $(CFLAGS) -c $<


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(SOURCES)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o $(TARGETS) *~ *.bak


# DEPENDENCIES: (do not delete this line)

xtablekey.o: ../../include/tablekey.h ../../include/array.h
xtablekey.o: ../../include/containerops.h ../../include/stats.h
xtablekey.o: ../../include/random.h ../../include/options.h
xtablekey.o: ../../include/str.h ../../include/strqueue.h
xtablekey.o: ../../include/parameter.h
xdatafile.o: ../../include/datafile.h ../../include/str.h
xdatafile.o: ../../include/strqueue.h ../../include/options.h
xdatafile.o: ../../include/parameter.h ../../include/tablekey.h
xdatafile.o: ../../include/array.h ../../include/containerops.h
xdatafile.o: ../../include/stats.h ../../include/random.h
xdatafile.o: ../../include/tabledata.h
copydata.o: ../../include/datafile.h ../../include/str.h
copydata.o: ../../include/strqueue.h ../../include/options.h
copydata.o: ../../include/parameter.h ../../include/tablekey.h
copydata.o: ../../include/array.h ../../include/containerops.h
copydata.o: ../../include/stats.h ../../include/random.h
copydata.o: ../../include/tabledata.h
processdata.o: ../../include/datafile.h ../../include/str.h
processdata.o: ../../include/strqueue.h ../../include/options.h
processdata.o: ../../include/parameter.h ../../include/tablekey.h
processdata.o: ../../include/array.h ../../include/containerops.h
processdata.o: ../../include/stats.h ../../include/random.h
processdata.o: ../../include/tabledata.h
