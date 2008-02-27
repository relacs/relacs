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

SOURCES = daqerror.cc tracespec.cc \
	  indata.cc inlist.cc \
	  outdata.cc outlist.cc \
          device.cc \
          analoginput.cc analogoutput.cc \
          attenuator.cc attenuate.cc \
          manipulator.cc temperature.cc \
	  acquire.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
HEADERS =
HEADERTARGETS = $(foreach header,$(HEADERS),../$(INCLUDEPATH)/$(header))
TEMPLATES = 
TEMPLATETARGETS = $(foreach template,$(TEMPLATES),../$(INCLUDEPATH)/$(template))
TARGET = libdaq.so
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TEMPLATETARGETS) $(HEADERTARGETS) $(TARGET)


../$(INCLUDEPATH)/%.h : %.h
	cp $< $@


$(TARGET): $(OBJECTS)
	$(CC) -shared -o $@ $+
	cp -p $@ ../$(LIBRARYPATH)


%.o: %.cc
	$(CC) -c $< $(CFLAGS)
	cp -p $(patsubst %.o,%.h,$@) ../$(INCLUDEPATH)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(SOURCES)


clean:
	rm -f *.o *.so *~ *.bak


# DEPENDENCIES: (do not delete this line)

daqerror.o: attenuate.h device.h attenuator.h daqerror.h
tracespec.o: outdata.h ../../include/sampledata.h ../../include/array.h
tracespec.o: ../../include/containerops.h ../../include/stats.h
tracespec.o: ../../include/random.h ../../include/linearrange.h
tracespec.o: ../../include/containerfuncs.h ../../include/spectrum.h
tracespec.o: ../../include/eventdata.h ../../include/detector.h
tracespec.o: ../../include/eventlist.h daqerror.h tracespec.h
indata.o: outdata.h ../../include/sampledata.h ../../include/array.h
indata.o: ../../include/containerops.h ../../include/stats.h
indata.o: ../../include/random.h ../../include/linearrange.h
indata.o: ../../include/containerfuncs.h ../../include/spectrum.h
indata.o: ../../include/eventdata.h ../../include/detector.h
indata.o: ../../include/eventlist.h daqerror.h indata.h
indata.o: ../../include/cyclicarray.h
inlist.o: inlist.h indata.h ../../include/cyclicarray.h
inlist.o: ../../include/sampledata.h ../../include/array.h
inlist.o: ../../include/containerops.h ../../include/stats.h
inlist.o: ../../include/random.h ../../include/linearrange.h
inlist.o: ../../include/containerfuncs.h ../../include/spectrum.h
inlist.o: ../../include/eventdata.h ../../include/detector.h
inlist.o: ../../include/eventlist.h daqerror.h
outdata.o: ../../include/random.h outdata.h ../../include/sampledata.h
outdata.o: ../../include/array.h ../../include/containerops.h
outdata.o: ../../include/stats.h ../../include/linearrange.h
outdata.o: ../../include/containerfuncs.h ../../include/spectrum.h
outdata.o: ../../include/eventdata.h ../../include/detector.h
outdata.o: ../../include/eventlist.h daqerror.h
outlist.o: outlist.h outdata.h ../../include/sampledata.h
outlist.o: ../../include/array.h ../../include/containerops.h
outlist.o: ../../include/stats.h ../../include/random.h
outlist.o: ../../include/linearrange.h ../../include/containerfuncs.h
outlist.o: ../../include/spectrum.h ../../include/eventdata.h
outlist.o: ../../include/detector.h ../../include/eventlist.h daqerror.h
device.o: device.h
analoginput.o: indata.h ../../include/cyclicarray.h
analoginput.o: ../../include/sampledata.h ../../include/array.h
analoginput.o: ../../include/containerops.h ../../include/stats.h
analoginput.o: ../../include/random.h ../../include/linearrange.h
analoginput.o: ../../include/containerfuncs.h ../../include/spectrum.h
analoginput.o: ../../include/eventdata.h ../../include/detector.h
analoginput.o: ../../include/eventlist.h daqerror.h analoginput.h device.h
analoginput.o: inlist.h tracespec.h
analogoutput.o: outdata.h ../../include/sampledata.h ../../include/array.h
analogoutput.o: ../../include/containerops.h ../../include/stats.h
analogoutput.o: ../../include/random.h ../../include/linearrange.h
analogoutput.o: ../../include/containerfuncs.h ../../include/spectrum.h
analogoutput.o: ../../include/eventdata.h ../../include/detector.h
analogoutput.o: ../../include/eventlist.h daqerror.h analogoutput.h device.h
analogoutput.o: outlist.h tracespec.h
attenuator.o: attenuator.h device.h
attenuate.o: attenuate.h device.h attenuator.h
manipulator.o: manipulator.h device.h
temperature.o: temperature.h device.h
acquire.o: acquire.h tracespec.h inlist.h indata.h
acquire.o: ../../include/cyclicarray.h ../../include/sampledata.h
acquire.o: ../../include/array.h ../../include/containerops.h
acquire.o: ../../include/stats.h ../../include/random.h
acquire.o: ../../include/linearrange.h ../../include/containerfuncs.h
acquire.o: ../../include/spectrum.h ../../include/eventdata.h
acquire.o: ../../include/detector.h ../../include/eventlist.h daqerror.h
acquire.o: outlist.h outdata.h analoginput.h device.h analogoutput.h
acquire.o: attenuate.h attenuator.h
