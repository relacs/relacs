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

SOURCES = rangeloop.cc \
          plugins.cc relacsplugin.cc \
          filter.cc filterdetectors.cc \
	  metadata.cc session.cc defaultsession.cc \
	  control.cc \
	  repro.cc defaultrepro.cc repros.cc macros.cc \
	  plottrace.cc savefiles.cc spiketrace.cc \
	  simulator.cc model.cc \
	  aisim.cc aosim.cc attsim.cc \
	  settings.cc relacswidget.cc relacsmain.cc
SOURCESHEADER = $(SOURCES:.cc=.h)
HEADERS = devicelist.h relacsdevices.h
HEADERTARGETS = $(foreach header,$(HEADERS),$(RELACSINC)/$(header))
MSOURCES = mrelacsplugin.cc \
           mfilter.cc mfilterdetectors.cc \
	   mmetadata.cc msession.cc mdefaultsession.cc mcontrol.cc \
	   mrepro.cc mdefaultrepro.cc mrepros.cc \
           mmacros.cc \
	   mplottrace.cc msavefiles.cc mspiketrace.cc \
	   msettings.cc mrelacswidget.cc
MOBJECTS = $(MSOURCES:.cc=.o)
OBJECTS = $(SOURCES:.cc=.o)
TARGET = librelacs.so
BACKUPFILES = Makefile



ORGRELACSINC = -I../include


.PHONY: all depend backupfiles clean


all: $(HEADERTARGETS) $(TARGET)


$(RELACSINC)/%.h : %.h
	cp -p $< $@


$(TARGET): $(OBJECTS) $(MOBJECTS)
	$(CC) -shared -o $@ $+
	cp -p $@ $(RELACSLIB)


$(OBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(QTINC)
	[ -f $(patsubst %.o,%.h,$@) ] && cp -p $(patsubst %.o,%.h,$@) $(RELACSINC); true

$(MOBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(QTINC)

$(MSOURCES): m%.cc: %.h
	$(MOC) $< -o $@


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) $(ORGRELACSINC) -- $(OBJECTS:.o=.cc) $(MOBJECTS:.o=.cc)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o *.so $(MSOURCES) $(MOBJECTS) *~ *.bak


# DEPENDENCIES: (do not delete this line)

rangeloop.o: ../include/random.h ../include/strqueue.h ../include/str.h
rangeloop.o: rangeloop.h
plugins.o: plugins.h
relacsplugin.o: ../include/messagebox.h metadata.h ../include/configclass.h
relacsplugin.o: ../include/strqueue.h ../include/str.h ../include/options.h
relacsplugin.o: ../include/parameter.h model.h ../include/cyclicarray.h
relacsplugin.o: ../include/sampledata.h ../include/array.h
relacsplugin.o: ../include/containerops.h ../include/stats.h
relacsplugin.o: ../include/random.h ../include/linearrange.h
relacsplugin.o: ../include/containerfuncs.h ../include/spectrum.h
relacsplugin.o: ../include/eventdata.h ../include/detector.h
relacsplugin.o: ../include/eventlist.h ../include/outdata.h
relacsplugin.o: ../include/daqerror.h relacsplugin.h ../include/tracespec.h
relacsplugin.o: ../include/indata.h ../include/inlist.h ../include/outlist.h
relacsplugin.o: plugins.h ../include/configdialog.h filter.h relacswidget.h
relacsplugin.o: ../include/configure.h ../include/config.h settings.h
relacsplugin.o: relacsdevices.h devicelist.h ../include/device.h
relacsplugin.o: ../include/analoginput.h ../include/analogoutput.h
relacsplugin.o: ../include/attenuator.h ../include/attenuate.h plottrace.h
relacsplugin.o: ../include/multiplot.h ../include/plot.h ../include/map.h
relacsplugin.o: ../include/acquire.h simulator.h savefiles.h
relacsplugin.o: ../include/tablekey.h repro.h spiketrace.h filterdetectors.h
relacsplugin.o: session.h control.h repros.h macros.h
filter.o: filter.h relacsplugin.h ../include/tracespec.h ../include/indata.h
filter.o: ../include/cyclicarray.h ../include/sampledata.h ../include/array.h
filter.o: ../include/containerops.h ../include/stats.h ../include/random.h
filter.o: ../include/linearrange.h ../include/containerfuncs.h
filter.o: ../include/spectrum.h ../include/eventdata.h ../include/detector.h
filter.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
filter.o: ../include/outdata.h ../include/outlist.h metadata.h
filter.o: ../include/configclass.h ../include/strqueue.h ../include/str.h
filter.o: ../include/options.h ../include/parameter.h plugins.h
filter.o: ../include/configdialog.h
filterdetectors.o: ../include/str.h repros.h ../include/options.h
filterdetectors.o: ../include/strqueue.h ../include/parameter.h
filterdetectors.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
filterdetectors.o: ../include/cyclicarray.h ../include/sampledata.h
filterdetectors.o: ../include/array.h ../include/containerops.h
filterdetectors.o: ../include/stats.h ../include/random.h
filterdetectors.o: ../include/linearrange.h ../include/containerfuncs.h
filterdetectors.o: ../include/spectrum.h ../include/eventdata.h
filterdetectors.o: ../include/detector.h ../include/eventlist.h
filterdetectors.o: ../include/daqerror.h ../include/inlist.h
filterdetectors.o: ../include/outdata.h ../include/outlist.h metadata.h
filterdetectors.o: ../include/configclass.h plugins.h
filterdetectors.o: ../include/configdialog.h filter.h session.h
filterdetectors.o: relacsdevices.h devicelist.h ../include/device.h
filterdetectors.o: ../include/analoginput.h ../include/analogoutput.h
filterdetectors.o: ../include/attenuator.h ../include/attenuate.h
filterdetectors.o: relacswidget.h ../include/configure.h ../include/config.h
filterdetectors.o: settings.h plottrace.h ../include/multiplot.h
filterdetectors.o: ../include/plot.h ../include/map.h ../include/acquire.h
filterdetectors.o: simulator.h savefiles.h ../include/tablekey.h repro.h
filterdetectors.o: spiketrace.h filterdetectors.h control.h macros.h
metadata.o: ../include/optdialog.h ../include/options.h ../include/str.h
metadata.o: ../include/strqueue.h ../include/parameter.h
metadata.o: ../include/optwidget.h relacswidget.h ../include/configclass.h
metadata.o: ../include/configure.h ../include/config.h settings.h
metadata.o: ../include/configdialog.h relacsdevices.h devicelist.h
metadata.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
metadata.o: ../include/cyclicarray.h ../include/sampledata.h
metadata.o: ../include/array.h ../include/containerops.h ../include/stats.h
metadata.o: ../include/random.h ../include/linearrange.h
metadata.o: ../include/containerfuncs.h ../include/spectrum.h
metadata.o: ../include/eventdata.h ../include/detector.h
metadata.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
metadata.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
metadata.o: ../include/device.h ../include/analoginput.h
metadata.o: ../include/analogoutput.h ../include/attenuator.h
metadata.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
metadata.o: ../include/plot.h ../include/map.h ../include/acquire.h
metadata.o: simulator.h savefiles.h ../include/tablekey.h repro.h
metadata.o: spiketrace.h filterdetectors.h session.h control.h repros.h
metadata.o: macros.h
session.o: ../include/str.h relacswidget.h ../include/configclass.h
session.o: ../include/strqueue.h ../include/options.h ../include/parameter.h
session.o: ../include/configure.h ../include/config.h settings.h
session.o: ../include/configdialog.h relacsdevices.h devicelist.h
session.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
session.o: ../include/cyclicarray.h ../include/sampledata.h
session.o: ../include/array.h ../include/containerops.h ../include/stats.h
session.o: ../include/random.h ../include/linearrange.h
session.o: ../include/containerfuncs.h ../include/spectrum.h
session.o: ../include/eventdata.h ../include/detector.h
session.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
session.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
session.o: ../include/device.h ../include/analoginput.h
session.o: ../include/analogoutput.h ../include/attenuator.h
session.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
session.o: ../include/plot.h ../include/map.h ../include/acquire.h
session.o: simulator.h savefiles.h ../include/tablekey.h repro.h spiketrace.h
session.o: filterdetectors.h session.h control.h repros.h macros.h
defaultsession.o: defaultsession.h control.h relacsplugin.h
defaultsession.o: ../include/tracespec.h ../include/indata.h
defaultsession.o: ../include/cyclicarray.h ../include/sampledata.h
defaultsession.o: ../include/array.h ../include/containerops.h
defaultsession.o: ../include/stats.h ../include/random.h
defaultsession.o: ../include/linearrange.h ../include/containerfuncs.h
defaultsession.o: ../include/spectrum.h ../include/eventdata.h
defaultsession.o: ../include/detector.h ../include/eventlist.h
defaultsession.o: ../include/daqerror.h ../include/inlist.h
defaultsession.o: ../include/outdata.h ../include/outlist.h metadata.h
defaultsession.o: ../include/configclass.h ../include/strqueue.h
defaultsession.o: ../include/str.h ../include/options.h
defaultsession.o: ../include/parameter.h plugins.h ../include/configdialog.h
control.o: ../include/str.h relacswidget.h ../include/configclass.h
control.o: ../include/strqueue.h ../include/options.h ../include/parameter.h
control.o: ../include/configure.h ../include/config.h settings.h
control.o: ../include/configdialog.h relacsdevices.h devicelist.h
control.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
control.o: ../include/cyclicarray.h ../include/sampledata.h
control.o: ../include/array.h ../include/containerops.h ../include/stats.h
control.o: ../include/random.h ../include/linearrange.h
control.o: ../include/containerfuncs.h ../include/spectrum.h
control.o: ../include/eventdata.h ../include/detector.h
control.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
control.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
control.o: ../include/device.h ../include/analoginput.h
control.o: ../include/analogoutput.h ../include/attenuator.h
control.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
control.o: ../include/plot.h ../include/map.h ../include/acquire.h
control.o: simulator.h savefiles.h ../include/tablekey.h repro.h spiketrace.h
control.o: filterdetectors.h session.h control.h repros.h macros.h
repro.o: ../include/str.h ../include/optdialog.h ../include/options.h
repro.o: ../include/strqueue.h ../include/parameter.h ../include/optwidget.h
repro.o: relacswidget.h ../include/configclass.h ../include/configure.h
repro.o: ../include/config.h settings.h ../include/configdialog.h
repro.o: relacsdevices.h devicelist.h relacsplugin.h ../include/tracespec.h
repro.o: ../include/indata.h ../include/cyclicarray.h ../include/sampledata.h
repro.o: ../include/array.h ../include/containerops.h ../include/stats.h
repro.o: ../include/random.h ../include/linearrange.h
repro.o: ../include/containerfuncs.h ../include/spectrum.h
repro.o: ../include/eventdata.h ../include/detector.h ../include/eventlist.h
repro.o: ../include/daqerror.h ../include/inlist.h ../include/outdata.h
repro.o: ../include/outlist.h metadata.h plugins.h ../include/device.h
repro.o: ../include/analoginput.h ../include/analogoutput.h
repro.o: ../include/attenuator.h ../include/attenuate.h plottrace.h
repro.o: ../include/multiplot.h ../include/plot.h ../include/map.h
repro.o: ../include/acquire.h simulator.h savefiles.h ../include/tablekey.h
repro.o: repro.h spiketrace.h filterdetectors.h session.h control.h repros.h
repro.o: macros.h
defaultrepro.o: defaultrepro.h repro.h ../include/outlist.h
defaultrepro.o: ../include/outdata.h ../include/sampledata.h
defaultrepro.o: ../include/array.h ../include/containerops.h
defaultrepro.o: ../include/stats.h ../include/random.h
defaultrepro.o: ../include/linearrange.h ../include/containerfuncs.h
defaultrepro.o: ../include/spectrum.h ../include/eventdata.h
defaultrepro.o: ../include/detector.h ../include/eventlist.h
defaultrepro.o: ../include/daqerror.h relacsplugin.h ../include/tracespec.h
defaultrepro.o: ../include/indata.h ../include/cyclicarray.h
defaultrepro.o: ../include/inlist.h metadata.h ../include/configclass.h
defaultrepro.o: ../include/strqueue.h ../include/str.h ../include/options.h
defaultrepro.o: ../include/parameter.h plugins.h ../include/configdialog.h
repros.o: ../include/messagebox.h ../include/optdialog.h ../include/options.h
repros.o: ../include/str.h ../include/strqueue.h ../include/parameter.h
repros.o: ../include/optwidget.h relacswidget.h ../include/configclass.h
repros.o: ../include/configure.h ../include/config.h settings.h
repros.o: ../include/configdialog.h relacsdevices.h devicelist.h
repros.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
repros.o: ../include/cyclicarray.h ../include/sampledata.h ../include/array.h
repros.o: ../include/containerops.h ../include/stats.h ../include/random.h
repros.o: ../include/linearrange.h ../include/containerfuncs.h
repros.o: ../include/spectrum.h ../include/eventdata.h ../include/detector.h
repros.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
repros.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
repros.o: ../include/device.h ../include/analoginput.h
repros.o: ../include/analogoutput.h ../include/attenuator.h
repros.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
repros.o: ../include/plot.h ../include/map.h ../include/acquire.h simulator.h
repros.o: savefiles.h ../include/tablekey.h repro.h spiketrace.h
repros.o: filterdetectors.h session.h control.h repros.h macros.h
repros.o: defaultrepro.h
macros.o: ../include/str.h ../include/strqueue.h ../include/optdialog.h
macros.o: ../include/options.h ../include/parameter.h ../include/optwidget.h
macros.o: ../include/messagebox.h rangeloop.h relacswidget.h
macros.o: ../include/configclass.h ../include/configure.h ../include/config.h
macros.o: settings.h ../include/configdialog.h relacsdevices.h devicelist.h
macros.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
macros.o: ../include/cyclicarray.h ../include/sampledata.h ../include/array.h
macros.o: ../include/containerops.h ../include/stats.h ../include/random.h
macros.o: ../include/linearrange.h ../include/containerfuncs.h
macros.o: ../include/spectrum.h ../include/eventdata.h ../include/detector.h
macros.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
macros.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
macros.o: ../include/device.h ../include/analoginput.h
macros.o: ../include/analogoutput.h ../include/attenuator.h
macros.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
macros.o: ../include/plot.h ../include/map.h ../include/acquire.h simulator.h
macros.o: savefiles.h ../include/tablekey.h repro.h spiketrace.h
macros.o: filterdetectors.h session.h control.h repros.h macros.h filter.h
plottrace.o: ../include/str.h relacswidget.h ../include/configclass.h
plottrace.o: ../include/strqueue.h ../include/options.h
plottrace.o: ../include/parameter.h ../include/configure.h
plottrace.o: ../include/config.h settings.h ../include/configdialog.h
plottrace.o: relacsdevices.h devicelist.h relacsplugin.h
plottrace.o: ../include/tracespec.h ../include/indata.h
plottrace.o: ../include/cyclicarray.h ../include/sampledata.h
plottrace.o: ../include/array.h ../include/containerops.h ../include/stats.h
plottrace.o: ../include/random.h ../include/linearrange.h
plottrace.o: ../include/containerfuncs.h ../include/spectrum.h
plottrace.o: ../include/eventdata.h ../include/detector.h
plottrace.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
plottrace.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
plottrace.o: ../include/device.h ../include/analoginput.h
plottrace.o: ../include/analogoutput.h ../include/attenuator.h
plottrace.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
plottrace.o: ../include/plot.h ../include/map.h ../include/acquire.h
plottrace.o: simulator.h savefiles.h ../include/tablekey.h repro.h
plottrace.o: spiketrace.h filterdetectors.h session.h control.h repros.h
plottrace.o: macros.h
savefiles.o: relacswidget.h ../include/configclass.h ../include/strqueue.h
savefiles.o: ../include/str.h ../include/options.h ../include/parameter.h
savefiles.o: ../include/configure.h ../include/config.h settings.h
savefiles.o: ../include/configdialog.h relacsdevices.h devicelist.h
savefiles.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
savefiles.o: ../include/cyclicarray.h ../include/sampledata.h
savefiles.o: ../include/array.h ../include/containerops.h ../include/stats.h
savefiles.o: ../include/random.h ../include/linearrange.h
savefiles.o: ../include/containerfuncs.h ../include/spectrum.h
savefiles.o: ../include/eventdata.h ../include/detector.h
savefiles.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
savefiles.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
savefiles.o: ../include/device.h ../include/analoginput.h
savefiles.o: ../include/analogoutput.h ../include/attenuator.h
savefiles.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
savefiles.o: ../include/plot.h ../include/map.h ../include/acquire.h
savefiles.o: simulator.h savefiles.h ../include/tablekey.h repro.h
savefiles.o: spiketrace.h filterdetectors.h session.h control.h repros.h
savefiles.o: macros.h
spiketrace.o: spiketrace.h
simulator.o: model.h ../include/cyclicarray.h ../include/sampledata.h
simulator.o: ../include/array.h ../include/containerops.h ../include/stats.h
simulator.o: ../include/random.h ../include/linearrange.h
simulator.o: ../include/containerfuncs.h ../include/spectrum.h
simulator.o: ../include/eventdata.h ../include/detector.h
simulator.o: ../include/eventlist.h ../include/outdata.h
simulator.o: ../include/daqerror.h relacsplugin.h ../include/tracespec.h
simulator.o: ../include/indata.h ../include/inlist.h ../include/outlist.h
simulator.o: metadata.h ../include/configclass.h ../include/strqueue.h
simulator.o: ../include/str.h ../include/options.h ../include/parameter.h
simulator.o: plugins.h ../include/configdialog.h simulator.h
simulator.o: ../include/acquire.h ../include/analoginput.h
simulator.o: ../include/device.h ../include/analogoutput.h
simulator.o: ../include/attenuate.h ../include/attenuator.h
model.o: relacswidget.h ../include/configclass.h ../include/strqueue.h
model.o: ../include/str.h ../include/options.h ../include/parameter.h
model.o: ../include/configure.h ../include/config.h settings.h
model.o: ../include/configdialog.h relacsdevices.h devicelist.h
model.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
model.o: ../include/cyclicarray.h ../include/sampledata.h ../include/array.h
model.o: ../include/containerops.h ../include/stats.h ../include/random.h
model.o: ../include/linearrange.h ../include/containerfuncs.h
model.o: ../include/spectrum.h ../include/eventdata.h ../include/detector.h
model.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
model.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
model.o: ../include/device.h ../include/analoginput.h
model.o: ../include/analogoutput.h ../include/attenuator.h
model.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
model.o: ../include/plot.h ../include/map.h ../include/acquire.h simulator.h
model.o: savefiles.h ../include/tablekey.h repro.h spiketrace.h
model.o: filterdetectors.h session.h control.h repros.h macros.h model.h
aisim.o: aisim.h ../include/analoginput.h ../include/device.h
aisim.o: ../include/inlist.h ../include/indata.h ../include/cyclicarray.h
aisim.o: ../include/sampledata.h ../include/array.h ../include/containerops.h
aisim.o: ../include/stats.h ../include/random.h ../include/linearrange.h
aisim.o: ../include/containerfuncs.h ../include/spectrum.h
aisim.o: ../include/eventdata.h ../include/detector.h ../include/eventlist.h
aisim.o: ../include/daqerror.h ../include/tracespec.h
aosim.o: ../include/analoginput.h ../include/device.h ../include/inlist.h
aosim.o: ../include/indata.h ../include/cyclicarray.h ../include/sampledata.h
aosim.o: ../include/array.h ../include/containerops.h ../include/stats.h
aosim.o: ../include/random.h ../include/linearrange.h
aosim.o: ../include/containerfuncs.h ../include/spectrum.h
aosim.o: ../include/eventdata.h ../include/detector.h ../include/eventlist.h
aosim.o: ../include/daqerror.h ../include/tracespec.h aosim.h
aosim.o: ../include/analogoutput.h ../include/outlist.h ../include/outdata.h
attsim.o: attsim.h ../include/attenuator.h ../include/device.h
settings.o: ../include/optwidget.h ../include/options.h ../include/str.h
settings.o: ../include/strqueue.h ../include/parameter.h relacswidget.h
settings.o: ../include/configclass.h ../include/configure.h
settings.o: ../include/config.h settings.h ../include/configdialog.h
settings.o: relacsdevices.h devicelist.h relacsplugin.h
settings.o: ../include/tracespec.h ../include/indata.h
settings.o: ../include/cyclicarray.h ../include/sampledata.h
settings.o: ../include/array.h ../include/containerops.h ../include/stats.h
settings.o: ../include/random.h ../include/linearrange.h
settings.o: ../include/containerfuncs.h ../include/spectrum.h
settings.o: ../include/eventdata.h ../include/detector.h
settings.o: ../include/eventlist.h ../include/daqerror.h ../include/inlist.h
settings.o: ../include/outdata.h ../include/outlist.h metadata.h plugins.h
settings.o: ../include/device.h ../include/analoginput.h
settings.o: ../include/analogoutput.h ../include/attenuator.h
settings.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
settings.o: ../include/plot.h ../include/map.h ../include/acquire.h
settings.o: simulator.h savefiles.h ../include/tablekey.h repro.h
settings.o: spiketrace.h filterdetectors.h session.h control.h repros.h
settings.o: macros.h
relacswidget.o: relacswidget.h ../include/configclass.h ../include/strqueue.h
relacswidget.o: ../include/str.h ../include/options.h ../include/parameter.h
relacswidget.o: ../include/configure.h ../include/config.h settings.h
relacswidget.o: ../include/configdialog.h relacsdevices.h devicelist.h
relacswidget.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
relacswidget.o: ../include/cyclicarray.h ../include/sampledata.h
relacswidget.o: ../include/array.h ../include/containerops.h
relacswidget.o: ../include/stats.h ../include/random.h
relacswidget.o: ../include/linearrange.h ../include/containerfuncs.h
relacswidget.o: ../include/spectrum.h ../include/eventdata.h
relacswidget.o: ../include/detector.h ../include/eventlist.h
relacswidget.o: ../include/daqerror.h ../include/inlist.h
relacswidget.o: ../include/outdata.h ../include/outlist.h metadata.h
relacswidget.o: plugins.h ../include/device.h ../include/analoginput.h
relacswidget.o: ../include/analogoutput.h ../include/attenuator.h
relacswidget.o: ../include/attenuate.h plottrace.h ../include/multiplot.h
relacswidget.o: ../include/plot.h ../include/map.h ../include/acquire.h
relacswidget.o: simulator.h savefiles.h ../include/tablekey.h repro.h
relacswidget.o: spiketrace.h filterdetectors.h session.h control.h repros.h
relacswidget.o: macros.h defaultsession.h aisim.h aosim.h attsim.h filter.h
relacswidget.o: model.h ../include/messagebox.h ../include/optdialog.h
relacswidget.o: ../include/optwidget.h
relacsmain.o: relacswidget.h ../include/configclass.h ../include/strqueue.h
relacsmain.o: ../include/str.h ../include/options.h ../include/parameter.h
relacsmain.o: ../include/configure.h ../include/config.h settings.h
relacsmain.o: ../include/configdialog.h relacsdevices.h devicelist.h
relacsmain.o: relacsplugin.h ../include/tracespec.h ../include/indata.h
relacsmain.o: ../include/cyclicarray.h ../include/sampledata.h
relacsmain.o: ../include/array.h ../include/containerops.h ../include/stats.h
relacsmain.o: ../include/random.h ../include/linearrange.h
relacsmain.o: ../include/containerfuncs.h ../include/spectrum.h
relacsmain.o: ../include/eventdata.h ../include/detector.h
relacsmain.o: ../include/eventlist.h ../include/daqerror.h
relacsmain.o: ../include/inlist.h ../include/outdata.h ../include/outlist.h
relacsmain.o: metadata.h plugins.h ../include/device.h
relacsmain.o: ../include/analoginput.h ../include/analogoutput.h
relacsmain.o: ../include/attenuator.h ../include/attenuate.h plottrace.h
relacsmain.o: ../include/multiplot.h ../include/plot.h ../include/map.h
relacsmain.o: ../include/acquire.h simulator.h savefiles.h
relacsmain.o: ../include/tablekey.h repro.h spiketrace.h filterdetectors.h
relacsmain.o: session.h control.h repros.h macros.h
