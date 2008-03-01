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
TOPDIR = ../..
include $(TOPDIR)/Makefile.h

CFLAGS += -I..
LIBRARIES = -L../../lib -ldaq -lnumerics

TARGETS = temp mm3a
SOURCES = $(foreach name,$(TARGETS),$(name).cc)
BACKUPFILES = Makefile


.PHONY: all depend backupfiles clean


all: $(TARGETS)


$(TARGETS): %: %.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBRARIES) -lm

temp : temp.o ../tempDTM5080.o
mm3a : mm3a.o ../kleindiek.o

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

temp.o: ../tempdtm5080.h
mm3a.o: ../kleindiek.h
