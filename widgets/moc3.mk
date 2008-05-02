moc_%.cc : $(HEADER_PATH)/%.h
	$(MOC) -i -o "$@" "$<"

clean-moc-extra:
	rm -vf moc_*.cc

clean-am: clean-moc-extra

