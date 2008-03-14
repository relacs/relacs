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
	  dynclampanaloginput.cc dynclampanalogoutput.cc \
	  comedianaloginput.cc comedianalogoutput.cc \
          comedihardware.cc \
	  cs3310nidio.cc cs3310serialnidio.cc cs3310pp.cc cs3310hardware.cc \
	  amplmode.cc \
	  kleindiek.cc \
	  tempdtm5080.cc \
	  hardware.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
HEADERS = nidaq.h moduledef.h
TARGET = libhardware.so libnieserieshardware.so libcomedihardware.so
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

libcomedihardware.so: comedianaloginput.o comedianalogoutput.o \
                      dynclampanaloginput.o dynclampanalogoutput.o \
		      comedihardware.o
	$(CC) -shared -o $@ $+ -lcomedi
	cp -p $@ tmp.so; mv tmp.so $(PLUGINLIBPATH)/$@;
	cp -p *.h $(PLUGININCPATH)


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

niai.o: niao.h nidaq.h ../include/analogoutput.h ../include/device.h
niai.o: ../include/outlist.h ../include/outdata.h ../include/sampledata.h
niai.o: ../include/array.h ../include/containerops.h ../include/stats.h
niai.o: ../include/random.h ../include/linearrange.h
niai.o: ../include/containerfuncs.h ../include/spectrum.h
niai.o: ../include/eventdata.h ../include/detector.h ../include/eventlist.h
niai.o: ../include/daqerror.h ../include/tracespec.h niai.h
niai.o: ../include/analoginput.h ../include/inlist.h ../include/indata.h
niai.o: ../include/cyclicarray.h
niao.o: ../include/stats.h niai.h nidaq.h ../include/analoginput.h
niao.o: ../include/device.h ../include/inlist.h ../include/indata.h
niao.o: ../include/cyclicarray.h ../include/sampledata.h ../include/array.h
niao.o: ../include/containerops.h ../include/random.h
niao.o: ../include/linearrange.h ../include/containerfuncs.h
niao.o: ../include/spectrum.h ../include/eventdata.h ../include/detector.h
niao.o: ../include/eventlist.h ../include/daqerror.h ../include/tracespec.h
niao.o: niao.h ../include/analogoutput.h ../include/outlist.h
niao.o: ../include/outdata.h
nidio.o: nidaq.h nidio.h ../include/device.h
nipfi.o: nidaq.h nipfi.h ../include/device.h
nieserieshardware.o: ../include/relacsplugin.h ../include/tracespec.h
nieserieshardware.o: ../include/indata.h ../include/cyclicarray.h
nieserieshardware.o: ../include/sampledata.h ../include/array.h
nieserieshardware.o: ../include/containerops.h ../include/stats.h
nieserieshardware.o: ../include/random.h ../include/linearrange.h
nieserieshardware.o: ../include/containerfuncs.h ../include/spectrum.h
nieserieshardware.o: ../include/eventdata.h ../include/detector.h
nieserieshardware.o: ../include/eventlist.h ../include/daqerror.h
nieserieshardware.o: ../include/inlist.h ../include/outdata.h
nieserieshardware.o: ../include/outlist.h ../include/metadata.h
nieserieshardware.o: ../include/config.h ../include/strqueue.h
nieserieshardware.o: ../include/str.h ../include/options.h
nieserieshardware.o: ../include/parameter.h ../include/plugins.h
nieserieshardware.o: ../include/configdialog.h niai.h nidaq.h
nieserieshardware.o: ../include/analoginput.h ../include/device.h niao.h
nieserieshardware.o: ../include/analogoutput.h nidio.h nipfi.h
nieserieshardware.o: cs3310serialnidio.h ../include/attenuator.h
nieserieshardware.o: cs3310nidio.h amplmode.h
dynclampanaloginput.o: dynclampanaloginput.h ../include/daqerror.h
dynclampanaloginput.o: ../include/analoginput.h ../include/device.h
dynclampanaloginput.o: ../include/inlist.h ../include/indata.h
dynclampanaloginput.o: ../include/cyclicarray.h ../include/sampledata.h
dynclampanaloginput.o: ../include/array.h ../include/containerops.h
dynclampanaloginput.o: ../include/stats.h ../include/random.h
dynclampanaloginput.o: ../include/linearrange.h ../include/containerfuncs.h
dynclampanaloginput.o: ../include/spectrum.h ../include/eventdata.h
dynclampanaloginput.o: ../include/detector.h ../include/eventlist.h
dynclampanaloginput.o: ../include/tracespec.h comedianaloginput.h
dynclampanaloginput.o: dynclampanalogoutput.h ../include/analogoutput.h
dynclampanaloginput.o: ../include/outlist.h ../include/outdata.h
dynclampanaloginput.o: comedianalogoutput.h moduledef.h
dynclampanalogoutput.o: dynclampanalogoutput.h ../include/daqerror.h
dynclampanalogoutput.o: ../include/analogoutput.h ../include/device.h
dynclampanalogoutput.o: ../include/outlist.h ../include/outdata.h
dynclampanalogoutput.o: ../include/sampledata.h ../include/array.h
dynclampanalogoutput.o: ../include/containerops.h ../include/stats.h
dynclampanalogoutput.o: ../include/random.h ../include/linearrange.h
dynclampanalogoutput.o: ../include/containerfuncs.h ../include/spectrum.h
dynclampanalogoutput.o: ../include/eventdata.h ../include/detector.h
dynclampanalogoutput.o: ../include/eventlist.h ../include/tracespec.h
dynclampanalogoutput.o: ../include/analoginput.h ../include/inlist.h
dynclampanalogoutput.o: ../include/indata.h ../include/cyclicarray.h
dynclampanalogoutput.o: comedianalogoutput.h moduledef.h
comedianaloginput.o: comedianalogoutput.h ../include/analogoutput.h
comedianaloginput.o: ../include/device.h ../include/outlist.h
comedianaloginput.o: ../include/outdata.h ../include/sampledata.h
comedianaloginput.o: ../include/array.h ../include/containerops.h
comedianaloginput.o: ../include/stats.h ../include/random.h
comedianaloginput.o: ../include/linearrange.h ../include/containerfuncs.h
comedianaloginput.o: ../include/spectrum.h ../include/eventdata.h
comedianaloginput.o: ../include/detector.h ../include/eventlist.h
comedianaloginput.o: ../include/daqerror.h ../include/tracespec.h
comedianaloginput.o: comedianaloginput.h ../include/analoginput.h
comedianaloginput.o: ../include/inlist.h ../include/indata.h
comedianaloginput.o: ../include/cyclicarray.h
comedianalogoutput.o: comedianaloginput.h ../include/analoginput.h
comedianalogoutput.o: ../include/device.h ../include/inlist.h
comedianalogoutput.o: ../include/indata.h ../include/cyclicarray.h
comedianalogoutput.o: ../include/sampledata.h ../include/array.h
comedianalogoutput.o: ../include/containerops.h ../include/stats.h
comedianalogoutput.o: ../include/random.h ../include/linearrange.h
comedianalogoutput.o: ../include/containerfuncs.h ../include/spectrum.h
comedianalogoutput.o: ../include/eventdata.h ../include/detector.h
comedianalogoutput.o: ../include/eventlist.h ../include/daqerror.h
comedianalogoutput.o: ../include/tracespec.h comedianalogoutput.h
comedianalogoutput.o: ../include/analogoutput.h ../include/outlist.h
comedianalogoutput.o: ../include/outdata.h
comedihardware.o: ../include/relacsplugin.h ../include/tracespec.h
comedihardware.o: ../include/indata.h ../include/cyclicarray.h
comedihardware.o: ../include/sampledata.h ../include/array.h
comedihardware.o: ../include/containerops.h ../include/stats.h
comedihardware.o: ../include/random.h ../include/linearrange.h
comedihardware.o: ../include/containerfuncs.h ../include/spectrum.h
comedihardware.o: ../include/eventdata.h ../include/detector.h
comedihardware.o: ../include/eventlist.h ../include/daqerror.h
comedihardware.o: ../include/inlist.h ../include/outdata.h
comedihardware.o: ../include/outlist.h ../include/metadata.h
comedihardware.o: ../include/config.h ../include/strqueue.h ../include/str.h
comedihardware.o: ../include/options.h ../include/parameter.h
comedihardware.o: ../include/plugins.h ../include/configdialog.h
comedihardware.o: comedianaloginput.h ../include/analoginput.h
comedihardware.o: ../include/device.h comedianalogoutput.h
comedihardware.o: ../include/analogoutput.h dynclampanaloginput.h
comedihardware.o: dynclampanalogoutput.h moduledef.h
cs3310nidio.o: cs3310nidio.h nidio.h ../include/device.h
cs3310nidio.o: ../include/attenuator.h
cs3310serialnidio.o: cs3310serialnidio.h nidio.h ../include/device.h
cs3310serialnidio.o: ../include/attenuator.h
cs3310pp.o: cs3310pp.h ../include/attenuator.h ../include/device.h
amplmode.o: amplmode.h nidio.h ../include/device.h
kleindiek.o: kleindiek.h ../include/manipulator.h ../include/device.h
tempdtm5080.o: tempdtm5080.h ../include/temperature.h ../include/device.h
hardware.o: ../include/relacsplugin.h ../include/tracespec.h
hardware.o: ../include/indata.h ../include/cyclicarray.h
hardware.o: ../include/sampledata.h ../include/array.h
hardware.o: ../include/containerops.h ../include/stats.h ../include/random.h
hardware.o: ../include/linearrange.h ../include/containerfuncs.h
hardware.o: ../include/spectrum.h ../include/eventdata.h
hardware.o: ../include/detector.h ../include/eventlist.h
hardware.o: ../include/daqerror.h ../include/inlist.h ../include/outdata.h
hardware.o: ../include/outlist.h ../include/metadata.h ../include/config.h
hardware.o: ../include/strqueue.h ../include/str.h ../include/options.h
hardware.o: ../include/parameter.h ../include/plugins.h
hardware.o: ../include/configdialog.h cs3310pp.h ../include/attenuator.h
hardware.o: ../include/device.h kleindiek.h ../include/manipulator.h
hardware.o: tempdtm5080.h ../include/temperature.h
