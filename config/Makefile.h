###############################################################################
#
# Makefile.h
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

# the C++ compiler:
CC = g++
# debug flags
DEBUGFLAGS = -g -Wall -Wno-non-virtual-dtor
NODEBUGFLAGS = -DNDEBUG
# flags for optimization
OPTFLAGS = -O2
NOOPTFLAGS =
# flags for the compiler:
ifdef RELEASE
CFLAGS = $(NODEBUGFLAGS) $(OPTFLAGS) $(OELFLAGS)
else
CFLAGS = $(DEBUGFLAGS) $(NOOPTFLAGS) $(OELFLAGS)
endif
# flags for linking a shared library:
CSHAREDFLAGS = -shared -Wall
# the linker
LD = ld
# the archiver to create static libraries (lib*.a)
AR = ar qcs

# add installed libraries to include file list:
CFLAGS += -I$(HOME)/include

# base path of Qt:
QTPATH = $(QTDIR)
ifeq ($(QTPATH),)
QTPATH = $(shell ls -d /usr/lib/qt* | tail -n 1)
endif
# the MOC (needed to compile Qt-Objects):
MOC = $(shell test -x $(QTPATH)/bin/moc && echo $(QTPATH)/bin/moc || echo moc)
# Qt include files:
QTINC = -I$(shell test -d $(QTPATH)/include/ && echo $(QTPATH)/include/ || (ls -d /usr/include/qt* | tail -n 1))
# Qt library:
QTLIB = $(shell test -d $(QTPATH)/lib/ && echo "-L$(QTPATH)/lib/ ")-lqt-mt
# flags for Qt
CFLAGS += -DQT_THREAD_SUPPORT

# make depend
DFLAGS = -fMakefile -Y -s"\# DEPENDENCIES: (do not delete this line)"

