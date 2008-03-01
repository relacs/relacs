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


TOPDIR = ../..

include $(TOPDIR)/Makefile.h


# Search for all *.cc files in the current directory.
# If any of your source file names begins with an 'm',
# then you have to add them manually to the SOURCES variable.
SOURCES := $(wildcard [!m]*.cc)
SOURCESHEADER = $(SOURCES:.cc=.h)
OBJECTS = $(SOURCES:.cc=.o)
# We need m*.cc files for each SOURCESHEADER file
# that contains a class inheriting QWidget or QObject.
# If every SOURCEHEADER file contains a QWiget or QObject
# then we can automatically generate the list of m*.cc files:
MSOURCES = $(foreach name,$(SOURCES),m$(name))
# Otherwise you have to specify them manually:
#MSOURCES = mmyfile.cc
MOBJECTS = $(MSOURCES:.cc=.o)
MPLUGINS = $(patsubst m%.cc,lib%.so,$(MSOURCES))
# strip .cc for targets:
PLUGINTARGETS := $(SOURCES:.cc=)
# transform these names into libXXX.so.
# PLUGINS is a list of the target plugin libraries.
PLUGINS = $(foreach name,$(PLUGINTARGETS),lib$(name).so)
# name of the current directory:
FOLDER = $(notdir $(CURDIR))
# some additional files to be backed up:
BACKUPFILES = Makefile

# path to the plugin libraries:
PLUGINLIBPATH = ../plugins
# path to the plugin header files:
PLUGININCPATH = ../include
# include pathes for the plugins:
PLUGININCLUDES = -I../include
# library pathes for the plugins:
PLUGINLIBRARIES = -L../plugins

# subdirectories which should be maked, too:
SUBDIRS = 


# Compile all Plugins found in the current directory and the sub directories:
.PHONY: all install backup backupfiles depend clean $(SUBDIRS) $(PLUGINTARGETS)
all: $(SUBDIRS) $(PLUGINS)

# compile a single plugin specified by its basename
# (the plugin libmyrepro.so can be compiled by make myrepro):
$(PLUGINTARGETS): %: $(SUBDIRS) lib%.so

# compile libraries in SUBDIRS first:
$(SUBDIRS):
	$(MAKE) -e -C $@


# Specify here the dependencies of your Plugins on additional libraries.
# See the RELACS documentation for details and examples.


# Let's compile the Plugins:
lib%.so: PLUGINLIB =
$(PLUGINS): lib%.so: %.o
	$(CC) -o $@ $^ $(CSHAREDFLAGS) $(PLUGINLIBRARIES) $(PLUGINLIB)
	cp -p $@ tmp.so; mv tmp.so $(PLUGINLIBPATH)/$@;
	cp -p $(patsubst lib%.so,%.h,$@) $(PLUGININCPATH)
$(OBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(RELACSINCLUDES) $(PLUGININCLUDES) $(QTINC)
$(MOBJECTS): %.o: %.cc
	$(CC) -c $< $(CFLAGS) $(RELACSINCLUDES) $(PLUGININCLUDES) $(QTINC)
$(MSOURCES): m%.cc: %.h
	$(MOC) $< -o $@
$(MPLUGINS): lib%.so: m%.o


# Backup the library source files in a tgz-archive:
backup:
	tar czvf $(FOLDER).tgz $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES)

# Create a list of files to be backed up in $(TOPDIR)/$(BACKUPFILESFILE):
backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


# Copy the libraries from the plugins directory to the working plugins directory: 
install:
	cp $(addprefix $(PLUGINLIBPATH)/,$(PLUGINS)) $(PLUGINSINSTALLPATH)
ifdef SUBDIRS
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir install; done
endif


# generate dependencies:
depend:
	makedepend $(DFLAGS) -- $(CDFLAGS) $(RELACSINCLUDES) $(PLUGININCLUDES) -- $(wildcard [!m]*.cc)
ifdef SUBDIRS
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir depend; done
endif


# Remove all object files, libraries, and backup files.
# Only leave the source files of the Plugins:
clean:
	rm -f $(OBJECTS) $(MSOURCES) $(MOBJECTS) $(PLUGINS) *~ *.bak *.tgz
	rm -f $(foreach name,$(PLUGINS),$(PLUGINLIBPATH)/$(name))
ifdef SUBDIRS
	for dir in $(SUBDIRS); do $(MAKE) -e -C $$dir clean; done
endif


# DEPENDENCIES: (do not delete this line)

spikedetector.o: spikedetector.h
