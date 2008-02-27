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


# search for all *.cc files in the current directory:
SOURCES := $(wildcard [!m]*.cc)
SOURCESHEADER = $(SOURCES:.cc=.h)
# strip .cc for targets:
PLUGINTARGETS := $(SOURCES:.cc=)
# transform these names into libXXX.so.
# PLUGINS is a list of the target plugin libraries.
PLUGINS = $(foreach name,$(PLUGINTARGETS),lib$(name).so)
# some additional files to be backed up:
BACKUPFILES = Makefile


# Compile all Plugins found in the current directory and the sub directories:
.PHONY: all install backup backupfiles doc help depend clean $(PLUGINTARGETS)
all: $(PLUGINS)

# compile a single plugin specified by its basename
# (the plugin libmyrepro.so can be compiled by make myrepro):
$(PLUGINTARGETS): %: lib%.so


# Specify here the dependencies of your Plugins on additional libraries.
# See the RELACS documentation for details and examples.


# Let's compile the Plugins:
lib%.so: PLUGINSLIB =
lib%.so: %.o m%.o
	$(CC) -o $@ $< m$< $(CSHAREDFLAGS) $(RELACSPLUGINSLIBRARIES) $(PLUGINSLIB)
	cp -p $@ tmp.so; mv tmp.so $(RELACSPLUGINS)/$@;
	cp -p $(patsubst lib%.so,%.h,$@) $(RELACSPLUGINSINC)
%.o: %.cc
	$(CC) -c $< $(CFLAGS) $(RELACSINCLUDES) $(RELACSPLUGINSINCLUDES) $(QTINC)
m%.cc: %.h
	$(MOC) $< -o $@


# Backup the library source files in a tgz-archive:
backup:
	tar czvf $(basename $(PWD)).tgz $$(find . -name "Makefile" -or -name "*.h" -or -name "[!m]*.cc" -or -name "*.c" )

# Create a list of files to be backed up in $(TOPDIR)/$(BACKUPFILESFILE):
backupfiles:
	for i in $(BACKUPFILES) $(SOURCESHEADER) $(SOURCES) $(HEADERS) $(TEMPLATES); \
	do echo $$i; done > $(TOPDIR)/$(BACKUPFILESFILE)


# Copy the libraries from the plugins directory to the working plugins directory: 
install:
	cp $(addprefix $(RELACSPLUGINS)/,$(PLUGINS)) $(PLUGINSINSTALLPATH)


# Create detector documentation:
doc:
	doxygen doc/detectors.dox

# Create detectors help files:
help: doc
	$(TOPDIR)/utils/createhelp $(TOPDIR)/plugins/help


# generate dependencies:
depend:
	makedepend $(DFLAGS) -- $(CFLAGS) $(RELACSINCLUDES) $(RELACSPLUGINSINCLUDES) -- $(wildcard [!m]*.cc)


# Remove all object files, libraries, and backup files.
# Only leave the source files of the Plugins:
clean:
	rm -f *.o m*.cc *.so *~ *.bak *.tgz
	rm -f $(foreach name,$(PLUGINS),$(RELACSPLUGINS)/$(name))


# DEPENDENCIES: (do not delete this line)

reproexample.o: reproexample.h ../include/repro.h ../include/options.h
reproexample.o: ../include/str.h ../include/strqueue.h ../include/parameter.h
reproexample.o: ../include/optwidget.h ../include/config.h
reproexample.o: ../include/outdata.h ../include/sampledarray.h
reproexample.o: ../include/array.h ../include/stats.h ../include/range.h
reproexample.o: ../include/outputdata.h ../include/multiplexeddata.h
reproexample.o: ../include/inputdata.h ../include/sampledata.h
reproexample.o: ../include/eventdata.h ../include/eventlist.h
reproexample.o: ../include/oelplugins.h ../include/plugins.h
