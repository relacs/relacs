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
LIBRARIES = -L../lib -lnumerics $(GSLLIB) $(RAND55LIB) -lm

TARGETS = xcontainerfuncs xarray xsampledata xsampledarray \
	  xkernel xrand xounoise xtests randtiming \
	  xspectrum coherence \
          linearfit simplexfit marquardtfit \
          euler generatepairs \
	  binrate syncevents
SOURCES = $(foreach name,$(TARGETS),$(name).cc)
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGETS)


$(TARGETS): %: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LIBRARIES)

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

xcontainerfuncs.o: ../../include/array.h ../../include/containerops.h
xcontainerfuncs.o: ../../include/stats.h ../../include/random.h
xcontainerfuncs.o: ../../include/containerfuncs.h
xarray.o: ../../include/array.h ../../include/containerops.h
xarray.o: ../../include/stats.h ../../include/random.h
xsampledata.o: ../../include/sampledata.h ../../include/array.h
xsampledata.o: ../../include/containerops.h ../../include/stats.h
xsampledata.o: ../../include/random.h ../../include/linearrange.h
xsampledata.o: ../../include/containerfuncs.h ../../include/spectrum.h
xsampledata.o: ../../include/eventdata.h ../../include/detector.h
xsampledata.o: ../../include/eventlist.h
xkernel.o: ../../include/kernel.h
xrand.o: ../../include/random.h
xounoise.o: ../../include/random.h ../../include/spectrum.h
xounoise.o: ../../include/sampledata.h ../../include/array.h
xounoise.o: ../../include/containerops.h ../../include/stats.h
xounoise.o: ../../include/linearrange.h ../../include/containerfuncs.h
xounoise.o: ../../include/eventdata.h ../../include/detector.h
xounoise.o: ../../include/eventlist.h
xtests.o: ../../include/random.h
randtiming.o: ../../include/random.h
xspectrum.o: ../../include/spectrum.h
coherence.o: ../../include/sampledata.h ../../include/array.h
coherence.o: ../../include/containerops.h ../../include/stats.h
coherence.o: ../../include/random.h ../../include/linearrange.h
coherence.o: ../../include/containerfuncs.h ../../include/spectrum.h
coherence.o: ../../include/eventdata.h ../../include/detector.h
coherence.o: ../../include/eventlist.h
linearfit.o: ../../include/array.h ../../include/containerops.h
linearfit.o: ../../include/stats.h ../../include/random.h
linearfit.o: ../../include/basisfunction.h ../../include/fitalgorithm.h
simplexfit.o: ../../include/array.h ../../include/containerops.h
simplexfit.o: ../../include/stats.h ../../include/random.h
simplexfit.o: ../../include/basisfunction.h ../../include/fitalgorithm.h
marquardtfit.o: ../../include/array.h ../../include/containerops.h
marquardtfit.o: ../../include/stats.h ../../include/random.h
marquardtfit.o: ../../include/basisfunction.h ../../include/fitalgorithm.h
euler.o: ../../include/odealgorithm.h
generatepairs.o: ../../include/random.h
binrate.o: ../../include/eventlist.h ../../include/random.h
binrate.o: ../../include/eventdata.h ../../include/array.h
binrate.o: ../../include/containerops.h ../../include/stats.h
binrate.o: ../../include/linearrange.h ../../include/sampledata.h
binrate.o: ../../include/containerfuncs.h ../../include/spectrum.h
binrate.o: ../../include/detector.h
syncevents.o: ../../include/eventlist.h ../../include/random.h
syncevents.o: ../../include/eventdata.h ../../include/array.h
syncevents.o: ../../include/containerops.h ../../include/stats.h
syncevents.o: ../../include/linearrange.h
