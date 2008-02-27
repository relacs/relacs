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

SOURCES = plot.cc multiplot.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
HEADERS = 
MSOURCES = $(foreach name,$(SOURCES),m$(name))
MOBJECTS = $(MSOURCES:.cc=.o)
TARGET = libmplot.so
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

plot.o: ../../include/str.h multiplot.h plot.h ../../include/array.h
plot.o: ../../include/containerops.h ../../include/stats.h
plot.o: ../../include/random.h ../../include/map.h ../../include/sampledata.h
plot.o: ../../include/linearrange.h ../../include/containerfuncs.h
plot.o: ../../include/spectrum.h ../../include/eventdata.h
plot.o: ../../include/detector.h ../../include/eventlist.h
plot.o: ../../include/indata.h ../../include/cyclicarray.h
plot.o: ../../include/daqerror.h
multiplot.o: ../../include/str.h multiplot.h plot.h ../../include/array.h
multiplot.o: ../../include/containerops.h ../../include/stats.h
multiplot.o: ../../include/random.h ../../include/map.h
multiplot.o: ../../include/sampledata.h ../../include/linearrange.h
multiplot.o: ../../include/containerfuncs.h ../../include/spectrum.h
multiplot.o: ../../include/eventdata.h ../../include/detector.h
multiplot.o: ../../include/eventlist.h ../../include/indata.h
multiplot.o: ../../include/cyclicarray.h ../../include/daqerror.h
