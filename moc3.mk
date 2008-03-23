MOC_CREATE = $(MOC) -i -o "$@" "$<"

hardware/moc_%.cc : hardware/include/relacs/%.h
	$(MOC_CREATE)

moc_%.cc : ../include/relacs/%.h
	$(MOC_CREATE)



clean-moc-extra:
	rm -vf moc_*.cc

clean-am: clean-moc-extra

