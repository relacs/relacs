SUBDIRS = $(RELACS_PLUGINS_SUBDIRS) 
DIST_SUBDIRS = $(RELACS_PLUGINS_DIST_SUBDIRS)

EXTRA_DIST = \
    relacshelp.mk \
    newplugin \
    newpluginset \
    skel

dist-hook:
	rm -rf `find $(distdir)/skel -name '.svn'`
