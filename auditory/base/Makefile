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
PLUGININCLUDES = -I../../hardware/include -I../../common/include -I../include
# library pathes for the plugins:
PLUGINLIBRARIES = -L../../hardware/plugins -L../../common/plugins -L../plugins

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
libauditoryneuron.so: PLUGINLIB = -lneuronmodels

libauditorysession.so: PLUGINLIB = -lnieserieshardware -lephystraces -lauditorytraces
libauditorysession.so: libauditorytraces.so

libauditorysearch.so: PLUGINLIB = -lauditorytraces
auditorysearch.o: libauditorytraces.so
libficurve.so: PLUGINLIB = -lephystraces -lauditorytraces
libfifield.so: PLUGINLIB = -lephystraces -lauditorytraces
libsinglestimulus.so: PLUGINLIB = -lephystraces -lauditorytraces
libspikeprecision.so: PLUGINLIB = -lephystraces -lauditorytraces
libsyslatency.so: PLUGINLIB = -lephystraces -lauditorytraces


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

auditoryneuron.o: ../../include/optwidget.h ../../include/options.h
auditoryneuron.o: ../../include/str.h ../../include/strqueue.h
auditoryneuron.o: ../../include/parameter.h ../../include/random.h
auditoryneuron.o: ../../include/odealgorithm.h auditoryneuron.h
auditorysearch.o: ../../include/tablekey.h ../../include/array.h
auditorysearch.o: ../../include/containerops.h ../../include/stats.h
auditorysearch.o: ../../include/random.h ../../include/options.h
auditorysearch.o: ../../include/str.h ../../include/strqueue.h
auditorysearch.o: ../../include/parameter.h ../../include/lcdrange.h
auditorysearch.o: auditorysearch.h auditorytraces.h
auditorysession.o: auditorysession.h ../../include/optwidget.h
auditorysession.o: ../../include/options.h ../../include/str.h
auditorysession.o: ../../include/strqueue.h ../../include/parameter.h
auditorysession.o: ../../include/multiplot.h ../../include/plot.h
auditorysession.o: ../../include/array.h ../../include/containerops.h
auditorysession.o: ../../include/stats.h ../../include/random.h
auditorysession.o: ../../include/map.h ../../include/sampledata.h
auditorysession.o: ../../include/linearrange.h ../../include/containerfuncs.h
auditorysession.o: ../../include/spectrum.h ../../include/eventdata.h
auditorysession.o: ../../include/detector.h ../../include/eventlist.h
auditorysession.o: ../../include/temperature.h ../../include/device.h
auditorysession.o: auditorytraces.h
auditorytraces.o: ../../include/str.h ../../include/inlist.h
auditorytraces.o: ../../include/indata.h ../../include/cyclicarray.h
auditorytraces.o: ../../include/sampledata.h ../../include/array.h
auditorytraces.o: ../../include/containerops.h ../../include/stats.h
auditorytraces.o: ../../include/random.h ../../include/linearrange.h
auditorytraces.o: ../../include/containerfuncs.h ../../include/spectrum.h
auditorytraces.o: ../../include/eventdata.h ../../include/detector.h
auditorytraces.o: ../../include/eventlist.h ../../include/daqerror.h
auditorytraces.o: ../../include/outdata.h auditorytraces.h
ficurve.o: ../../include/array.h ../../include/containerops.h
ficurve.o: ../../include/stats.h ../../include/random.h ../../include/map.h
ficurve.o: ../../include/sampledata.h ../../include/linearrange.h
ficurve.o: ../../include/containerfuncs.h ../../include/spectrum.h
ficurve.o: ../../include/eventdata.h ../../include/detector.h
ficurve.o: ../../include/eventlist.h ../../include/str.h
ficurve.o: ../../include/tablekey.h ../../include/options.h
ficurve.o: ../../include/strqueue.h ../../include/parameter.h
ficurve.o: auditorysession.h ../../include/optwidget.h
ficurve.o: ../../include/multiplot.h ../../include/plot.h
ficurve.o: ../../include/temperature.h ../../include/device.h
ficurve.o: auditorytraces.h ficurve.h ../../include/rangeloop.h
fifield.o: ../../include/str.h ../../include/tablekey.h ../../include/array.h
fifield.o: ../../include/containerops.h ../../include/stats.h
fifield.o: ../../include/random.h ../../include/options.h
fifield.o: ../../include/strqueue.h ../../include/parameter.h
fifield.o: auditorysession.h ../../include/optwidget.h
fifield.o: ../../include/multiplot.h ../../include/plot.h ../../include/map.h
fifield.o: ../../include/sampledata.h ../../include/linearrange.h
fifield.o: ../../include/containerfuncs.h ../../include/spectrum.h
fifield.o: ../../include/eventdata.h ../../include/detector.h
fifield.o: ../../include/eventlist.h ../../include/temperature.h
fifield.o: ../../include/device.h auditorytraces.h fifield.h
fifield.o: ../../include/rangeloop.h ficurve.h
singlestimulus.o: ../../include/optwidget.h ../../include/options.h
singlestimulus.o: ../../include/str.h ../../include/strqueue.h
singlestimulus.o: ../../include/parameter.h ../../include/tablekey.h
singlestimulus.o: ../../include/array.h ../../include/containerops.h
singlestimulus.o: ../../include/stats.h ../../include/random.h
singlestimulus.o: ../../include/translate.h ../../include/kernel.h
singlestimulus.o: auditorysession.h ../../include/multiplot.h
singlestimulus.o: ../../include/plot.h ../../include/map.h
singlestimulus.o: ../../include/sampledata.h ../../include/linearrange.h
singlestimulus.o: ../../include/containerfuncs.h ../../include/spectrum.h
singlestimulus.o: ../../include/eventdata.h ../../include/detector.h
singlestimulus.o: ../../include/eventlist.h ../../include/temperature.h
singlestimulus.o: ../../include/device.h auditorytraces.h singlestimulus.h
spikeprecision.o: ../../include/kernel.h ../../include/spectrum.h
spikeprecision.o: ../../include/tablekey.h ../../include/array.h
spikeprecision.o: ../../include/containerops.h ../../include/stats.h
spikeprecision.o: ../../include/random.h ../../include/options.h
spikeprecision.o: ../../include/str.h ../../include/strqueue.h
spikeprecision.o: ../../include/parameter.h ../../include/translate.h
spikeprecision.o: auditorysession.h ../../include/optwidget.h
spikeprecision.o: ../../include/multiplot.h ../../include/plot.h
spikeprecision.o: ../../include/map.h ../../include/sampledata.h
spikeprecision.o: ../../include/linearrange.h ../../include/containerfuncs.h
spikeprecision.o: ../../include/eventdata.h ../../include/detector.h
spikeprecision.o: ../../include/eventlist.h ../../include/temperature.h
spikeprecision.o: ../../include/device.h auditorytraces.h spikeprecision.h
spikeprecision.o: ../../include/rangeloop.h
syslatency.o: ../../include/tablekey.h ../../include/array.h
syslatency.o: ../../include/containerops.h ../../include/stats.h
syslatency.o: ../../include/random.h ../../include/options.h
syslatency.o: ../../include/str.h ../../include/strqueue.h
syslatency.o: ../../include/parameter.h ../../include/sampledata.h
syslatency.o: ../../include/linearrange.h ../../include/containerfuncs.h
syslatency.o: ../../include/spectrum.h ../../include/eventdata.h
syslatency.o: ../../include/detector.h ../../include/eventlist.h
syslatency.o: auditorysession.h ../../include/optwidget.h
syslatency.o: ../../include/multiplot.h ../../include/plot.h
syslatency.o: ../../include/map.h ../../include/temperature.h
syslatency.o: ../../include/device.h auditorytraces.h syslatency.h
