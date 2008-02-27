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

LIBRARIES = -L$(HOME)/lib -loptions -ldatafile -lnumerics -ldaq -lmplot

TARGETS = xplot xplots
SOURCES = $(foreach name,$(TARGETS),$(name).cc)
HEADERS = 
MSOURCES = 
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGETS)


%: %.o
	$(CC) $(CFLAGS) -o $@ $+ $(QTLIB) $(LIBRARIES) -lm

%.o: %.cc
	$(CC) $(CFLAGS) -c $< $(QTINC)

$(MSOURCES): m%.cc: %.h
	$(MOC) $< -o $@


depend:
	makedepend $(DFLAGS) -- $(CFLAGS) -- $(SOURCES)


backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


clean:
	rm -f *.o $(TARGETS) *~ *.bak


xoptwidget : mmainwidget.o

# DEPENDENCIES: (do not delete this line)

xplot.o: ../../include/map.h ../../include/containerops.h
xplot.o: ../../include/array.h ../../include/stats.h ../../include/random.h
xplot.o: ../../include/sampledata.h ../../include/linearrange.h
xplot.o: ../../include/containerfuncs.h ../../include/spectrum.h
xplot.o: ../../include/eventdata.h ../../include/detector.h
xplot.o: ../../include/eventlist.h ../../include/plot.h
xplot.o: ../../include/indata.h ../../include/cyclicarray.h
xplot.o: ../../include/daqerror.h
xplots.o: ../../include/multiplot.h ../../include/plot.h
xplots.o: ../../include/array.h ../../include/containerops.h
xplots.o: ../../include/stats.h ../../include/random.h ../../include/map.h
xplots.o: ../../include/sampledata.h ../../include/linearrange.h
xplots.o: ../../include/containerfuncs.h ../../include/spectrum.h
xplots.o: ../../include/eventdata.h ../../include/detector.h
xplots.o: ../../include/eventlist.h ../../include/indata.h
xplots.o: ../../include/cyclicarray.h ../../include/daqerror.h
