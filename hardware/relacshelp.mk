# relacshelp.mk, (c) 2008 Jan Benda
# Makefile targets for integrating the generation of plugin help files

help/helpstamp: 
	$(MKDIR_P) help
	../utils/relacshelp doc/html help $(PLUGINSET)
	touch help/helpstamp

help-run: help/helpstamp

help-make: help-clean help-run

help-clean:
	-rm -f -r help

help-install: help-run
	test -z "$(pkgdatadir)" || $(MKDIR_P) "$(DESTDIR)$(pkgdatadir)"
	$(MKDIR_P) "$(DESTDIR)$(pkgdatadir)/help"
	$(INSTALL_DATA) help/*.* "$(DESTDIR)$(pkgdatadir)/help"

help-uninstall:
	-rm -f -r "$(DESTDIR)$(pkgdatadir)/help"
