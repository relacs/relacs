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

TOPDIR = ../..
include $(TOPDIR)/Makefile.h

CFLAGS += -I../include
LIBRARIES = -L../lib -loptions -ldatafile -L ../../lib -lnumerics -lgsl -lgslcblas
INSTALLPATH = $(HOME)/bin

TARGETS = fixdata datainfo datacolumn \
          selectdata splitdata mergedata \
          bindata datastats plotdata
SOURCES = $(foreach name,$(TARGETS),$(name).cc)
BACKUPFILES = Makefile


.PHONY: all depend install backup backupfiles clean


all: $(TARGETS)


$(TARGETS): %: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBRARIES) -lm

%.o: %.cc
	$(CC) $(CFLAGS) -c $<


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(SOURCES)


install: all
	cp $(TARGETS) $(INSTALLPATH)


backup:
	tar czvf datatools.tgz *[!~]


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o $(TARGETS) *.tgz *~ *.bak


# DEPENDENCIES: (do not delete this line)

fixdata.o: ../../include/datafile.h ../../include/str.h
fixdata.o: ../../include/strqueue.h ../../include/options.h
fixdata.o: ../../include/parameter.h ../../include/tablekey.h
fixdata.o: ../../include/array.h ../../include/containerops.h
fixdata.o: ../../include/stats.h ../../include/random.h
fixdata.o: ../../include/tabledata.h
datainfo.o: ../../include/datafile.h ../../include/str.h
datainfo.o: ../../include/strqueue.h ../../include/options.h
datainfo.o: ../../include/parameter.h ../../include/tablekey.h
datainfo.o: ../../include/array.h ../../include/containerops.h
datainfo.o: ../../include/stats.h ../../include/random.h
datainfo.o: ../../include/tabledata.h
datacolumn.o: ../../include/str.h ../../include/datafile.h
datacolumn.o: ../../include/strqueue.h ../../include/options.h
datacolumn.o: ../../include/parameter.h ../../include/tablekey.h
datacolumn.o: ../../include/array.h ../../include/containerops.h
datacolumn.o: ../../include/stats.h ../../include/random.h
datacolumn.o: ../../include/tabledata.h
selectdata.o: ../../include/str.h ../../include/strqueue.h
selectdata.o: ../../include/datafile.h ../../include/options.h
selectdata.o: ../../include/parameter.h ../../include/tablekey.h
selectdata.o: ../../include/array.h ../../include/containerops.h
selectdata.o: ../../include/stats.h ../../include/random.h
selectdata.o: ../../include/tabledata.h
splitdata.o: ../../include/str.h ../../include/strqueue.h
splitdata.o: ../../include/options.h ../../include/parameter.h
splitdata.o: ../../include/datafile.h ../../include/tablekey.h
splitdata.o: ../../include/array.h ../../include/containerops.h
splitdata.o: ../../include/stats.h ../../include/random.h
splitdata.o: ../../include/tabledata.h ../../include/translate.h
mergedata.o: ../../include/str.h ../../include/strqueue.h
mergedata.o: ../../include/datafile.h ../../include/options.h
mergedata.o: ../../include/parameter.h ../../include/tablekey.h
mergedata.o: ../../include/array.h ../../include/containerops.h
mergedata.o: ../../include/stats.h ../../include/random.h
mergedata.o: ../../include/tabledata.h
bindata.o: ../../include/str.h ../../include/datafile.h
bindata.o: ../../include/strqueue.h ../../include/options.h
bindata.o: ../../include/parameter.h ../../include/tablekey.h
bindata.o: ../../include/array.h ../../include/containerops.h
bindata.o: ../../include/stats.h ../../include/random.h
bindata.o: ../../include/tabledata.h
datastats.o: ../../include/str.h ../../include/datafile.h
datastats.o: ../../include/strqueue.h ../../include/options.h
datastats.o: ../../include/parameter.h ../../include/tablekey.h
datastats.o: ../../include/array.h ../../include/containerops.h
datastats.o: ../../include/stats.h ../../include/random.h
datastats.o: ../../include/tabledata.h ../../include/statstests.h
plotdata.o: ../../include/str.h ../../include/strqueue.h
plotdata.o: ../../include/options.h ../../include/parameter.h
plotdata.o: ../../include/tablekey.h ../../include/array.h
plotdata.o: ../../include/containerops.h ../../include/stats.h
plotdata.o: ../../include/random.h ../../include/datafile.h
plotdata.o: ../../include/tabledata.h ../../include/translate.h
