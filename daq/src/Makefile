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
tracespec.o: outdata.h daqerror.h tracespec.h
indata.o: outdata.h daqerror.h indata.h
inlist.o: inlist.h indata.h daqerror.h
outdata.o: outdata.h daqerror.h
outlist.o: outlist.h outdata.h daqerror.h
device.o: device.h
analoginput.o: indata.h daqerror.h analoginput.h device.h inlist.h
analoginput.o: tracespec.h
analogoutput.o: outdata.h daqerror.h analogoutput.h device.h outlist.h
analogoutput.o: tracespec.h
attenuator.o: attenuator.h device.h
attenuate.o: attenuate.h device.h attenuator.h
manipulator.o: manipulator.h device.h
temperature.o: temperature.h device.h
acquire.o: acquire.h tracespec.h inlist.h indata.h daqerror.h outlist.h
acquire.o: outdata.h analoginput.h device.h analogoutput.h attenuate.h
acquire.o: attenuator.h
