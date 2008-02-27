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


export

# the C++ compiler:
CC = g++
# flags for the compiler:
CFLAGS = -g -Wall # -O2
# flags for linking a shared library:
CSHAREDFLAGS = -shared -Wall

# base path of Qt:
QTPATH = /usr/lib/qt-2.3.1
# the MOC (needed to compile Qt-Objects):
MOC = $(QTPATH)/bin/moc
# Qt include files:
QTINC = -I$(QTPATH)/include/
# Qt library:
QTLIB = -L$(QTPATH)/lib/ -lqt

# base path of RELACS:
RELACSPATH = $(PWD)/..
# RELACS include files:
RELACSINC = $(RELACSPATH)/include
# RELACS libraries:
RELACSLIB = $(RELACSPATH)/lib
# include for RELACS
RELACSINCLUDES = -I$(RELACSINC)
# libraries for RELACS:
RELACSLIBRARIES = -L$(RELACSLIB) -ldatahandling -lnewwidgets -lrelacsclasses -lhardware -ldl

# RELACS plugins:
RELACSPLUGINS = $(RELACSPATH)/plugins
# RELACS includes for plugins:
RELACSPLUGINSINC = $(RELACSPLUGINS)/include
# RELACS libraries for plugins:
RELACSPLUGINSLIB = $(RELACSPLUGINS)/lib
# include for RELACS plugins:
RELACSPLUGINSINCLUDES = -I$(RELACSPLUGINSINC)
# own libraries of a plugins:
PLUGINSLIBRARIES = -L. -L$(RELACSPLUGINSLIB)
PLUGINSLIB =

# base path for installation of RELACS:
INSTALLPATH = ~/bin
# plugins-path where plugins should be installed:
PLUGINSINSTALLPATH = $(INSTALLPATH)/plugins
# plugins-path where libraries for plugins should be installed:
PLUGINSLIBINSTALLPATH = $(PLUGINSINSTALLPATH)/lib
# plugins-path where includes for plugins should be installed:
PLUGINSINCINSTALLPATH = $(PLUGINSINSTALLPATH)/include

# search path for RELACS includes:
VPATH = $(RELACSINC):$(RELACSPLUGINSINC):


# objects to compile:
TARGETS = bin2dat checkao uw2sw data
SOURCES = bin2dat.c checkao.cc uw2sw.c data.c
BACKUPFILES = Makefile Makefile.plugins createhelp createweb \
	      relacsscript RELACS.kdelnk \
	      bin2wav \
	      scaninf scaninf.doc \
	      addgpl \
	      updatefiles \
	      remotebackup \
	      autoemailbugs stopjobs \
	      diffrelacs lcc.pl replace.pl

.PHONY: all install backup backupfiles clean
all: $(TARGETS)

bin2dat: bin2dat.c
	$(CC) $(CFLAGS) -o $@ $<

uw2sw: uw2sw.c
	$(CC) $(CFLAGS) -o $@ $<

checkao: checkao.cc
	$(CC) $(CFLAGS) -o $@ $< $(RELACSINCLUDES) $(QTINC) $(RELACSLIBRARIES) $(QTLIB)


# Backup the library source files in a tgz-archive:
backup:
	tar czvf $(basename $(PWD)).tgz $$(find . -name "Makefile" -or -name "*.hh" -or -name "*.cc" -or -name "*.h" -or -name "*.c" -name "*.doc" )

# Create a list of files to be backed up in $(TOPDIR)/$(BACKUPFILESFILE):
backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


# Copy the libraries from the plugins directory to the working plugins directory: 
install:
	cp $(TARGETS) $(INSTALLPATH)


# Remove all object files, libraries, and backup files.
# Only leave the sourve files of the RePros:
clean:
	rm -f *.o m*.cc *.so *~ *.tgz $(TARGETS)
