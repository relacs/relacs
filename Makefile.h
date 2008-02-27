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


# Version number of RELACS:
VERSION = 0.9.6
# Date of last compilation:
DATE = $(shell date '+%D')

# the C++ compiler:
CC = g++
# debug flags
DEBUGFLAGS = -g -Wall -Wno-non-virtual-dtor 
NODEBUGFLAGS = -DNDEBUG
# flags for optimization
OPTFLAGS = -O2
NOOPTFLAGS =
# gsl library present?
GSL = $(shell test -f /usr/lib/libgsl.so -o -f /usr/local/lib/libgsl.so && echo "GSL")
ifneq ($(GSL),)
  GSLFLAGS = -DGSL
  GSLLIB = -lgsl -lgslcblas
endif
# rand55 library present?
RAND55 = $(shell test -f /usr/lib/librand55.so -o -f /usr/local/lib/librand55.so && echo "RAND55")
ifneq ($(RAND55),)
  RAND55FLAGS = -DRAND55
  RAND55LIB = -lrand55
endif
# flags for RELACS
RELACSFLAGS = "-DRELACSVERSION=\"$(VERSION) ($(DATE))\""
RELACSFLAGS += "-DPLUGINVERSION=\"$(VERSION)\""
RELACSFLAGS += -DRELACSDATA
# flags for the compiler:
ifdef RELEASE
CFLAGS = $(NODEBUGFLAGS) $(OPTFLAGS) $(GSLFLAGS) $(RAND55FLAGS) $(RELACSFLAGS)
else
CFLAGS = $(DEBUGFLAGS) $(NOOPTFLAGS) $(GSLFLAGS) $(RAND55FLAGS) $(RELACSFLAGS)
endif
# flags for linking a shared library:
CSHAREDFLAGS = -shared -Wall
# the linker
LD = ld
# the archiver to create static libraries (lib*.a)
AR = ar qcs

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

# Dynamic libraries:
DLLIB = -ldl

# make depend
DFLAGS = -fMakefile -Y -s"\# DEPENDENCIES: (do not delete this line)"

# RELACS include files:
RELACSINC = $(TOPDIR)/include
# include for RELACS
RELACSINCLUDES = -I$(RELACSINC)
CFLAGS += $(RELACSINCLUDES)
# RELACS libraries:
RELACSLIB = $(TOPDIR)/lib

# base path for installation of RELACS:
INSTALLPATH = /extra/setup/relacs
# path for installation of the RELACS binary:
RELACSINSTALLPATH = $(INSTALLPATH)

# plugins-path where plugins should be installed:
PLUGINSINSTALLPATH = $(INSTALLPATH)/plugins
# plugins-path where libraries for plugins should be installed:
PLUGINSLIBINSTALLPATH = $(PLUGINSINSTALLPATH)/lib
# plugins-path where includes for plugins should be installed:
PLUGINSINCINSTALLPATH = $(PLUGINSINSTALLPATH)/include
