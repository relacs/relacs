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

SOURCES = array.cc linearrange.cc sampledata.cc \
          kernel.cc basisfunction.cc random.cc spectrum.cc statstests.cc \
	  fitalgorithm.cc \
	  eventdata.cc eventlist.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
TEMPLATES = map.h cyclicarray.h cyclicsampledata.h \
            odealgorithm.h stats.h containerops.h containerfuncs.h \
            detector.h
TEMPLATETARGETS = $(foreach template,$(TEMPLATES),../$(INCLUDEPATH)/$(template))
TARGET = libnumerics.so
BACKUPFILES = Makefile Makefile.h


.PHONY: all depend backupfiles clean


all: $(TEMPLATETARGETS) $(TARGET)


../$(INCLUDEPATH)/%.h : %.h
	cp $< $@


$(TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $+ $(GSLLIB) $(RAND55LIB)
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
	rm -f *.o *.so *~ *.bak


# DEPENDENCIES: (do not delete this line)

array.o: array.h containerops.h stats.h random.h
linearrange.o: linearrange.h
sampledata.o: sampledata.h array.h containerops.h stats.h random.h
sampledata.o: linearrange.h containerfuncs.h spectrum.h eventdata.h
sampledata.o: detector.h eventlist.h
kernel.o: kernel.h
basisfunction.o: basisfunction.h array.h containerops.h stats.h random.h
random.o: random.h
spectrum.o: spectrum.h
statstests.o: stats.h statstests.h array.h containerops.h random.h
fitalgorithm.o: fitalgorithm.h array.h containerops.h stats.h random.h
eventdata.o: sampledata.h array.h containerops.h stats.h random.h
eventdata.o: linearrange.h containerfuncs.h spectrum.h eventdata.h detector.h
eventdata.o: eventlist.h kernel.h
eventlist.o: array.h containerops.h stats.h random.h sampledata.h
eventlist.o: linearrange.h containerfuncs.h spectrum.h eventdata.h detector.h
eventlist.o: eventlist.h kernel.h
