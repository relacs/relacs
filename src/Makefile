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

rangeloop.o: rangeloop.h
plugins.o: plugins.h
relacsplugin.o: metadata.h model.h relacsplugin.h plugins.h filter.h
relacsplugin.o: relacswidget.h settings.h relacsdevices.h devicelist.h
relacsplugin.o: plottrace.h simulator.h savefiles.h repro.h spiketrace.h
relacsplugin.o: filterdetectors.h session.h control.h repros.h macros.h
filter.o: filter.h relacsplugin.h metadata.h plugins.h
filterdetectors.o: repros.h relacsplugin.h metadata.h plugins.h filter.h
filterdetectors.o: session.h relacsdevices.h devicelist.h relacswidget.h
filterdetectors.o: settings.h plottrace.h simulator.h savefiles.h repro.h
filterdetectors.o: spiketrace.h filterdetectors.h control.h macros.h
metadata.o: relacswidget.h settings.h relacsdevices.h devicelist.h
metadata.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
metadata.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
metadata.o: control.h repros.h macros.h
session.o: relacswidget.h settings.h relacsdevices.h devicelist.h
session.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
session.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
session.o: control.h repros.h macros.h
defaultsession.o: defaultsession.h control.h relacsplugin.h metadata.h
defaultsession.o: plugins.h
control.o: relacswidget.h settings.h relacsdevices.h devicelist.h
control.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
control.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
control.o: control.h repros.h macros.h
repro.o: relacswidget.h settings.h relacsdevices.h devicelist.h
repro.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
repro.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
repro.o: control.h repros.h macros.h
defaultrepro.o: defaultrepro.h repro.h relacsplugin.h metadata.h plugins.h
repros.o: relacswidget.h settings.h relacsdevices.h devicelist.h
repros.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
repros.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
repros.o: control.h repros.h macros.h defaultrepro.h
macros.o: rangeloop.h relacswidget.h settings.h relacsdevices.h devicelist.h
macros.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
macros.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
macros.o: control.h repros.h macros.h filter.h
plottrace.o: relacswidget.h settings.h relacsdevices.h devicelist.h
plottrace.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
plottrace.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
plottrace.o: control.h repros.h macros.h
savefiles.o: relacswidget.h settings.h relacsdevices.h devicelist.h
savefiles.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
savefiles.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
savefiles.o: control.h repros.h macros.h
spiketrace.o: spiketrace.h
simulator.o: model.h relacsplugin.h metadata.h plugins.h simulator.h
model.o: relacswidget.h settings.h relacsdevices.h devicelist.h
model.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
model.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
model.o: control.h repros.h macros.h model.h
aisim.o: aisim.h
aosim.o: aosim.h
attsim.o: attsim.h
settings.o: relacswidget.h settings.h relacsdevices.h devicelist.h
settings.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
settings.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
settings.o: control.h repros.h macros.h
relacswidget.o: relacswidget.h settings.h relacsdevices.h devicelist.h
relacswidget.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
relacswidget.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
relacswidget.o: control.h repros.h macros.h defaultsession.h aisim.h aosim.h
relacswidget.o: attsim.h filter.h model.h
relacsmain.o: relacswidget.h settings.h relacsdevices.h devicelist.h
relacsmain.o: relacsplugin.h metadata.h plugins.h plottrace.h simulator.h
relacsmain.o: savefiles.h repro.h spiketrace.h filterdetectors.h session.h
relacsmain.o: control.h repros.h macros.h
