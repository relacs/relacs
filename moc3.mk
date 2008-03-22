MOC_CREATE = $(MOC) -i -o "$@" "$<"

hardware/moc_%.cc : hardware/include/relacs/%.h
	$(MOC_CREATE)

moc_%.cc : ../include/relacs/%.h
	$(MOC_CREATE)



clean-moc-extra:
	-rm -v -f *-moc.{cpp,o,lo}

clean: clean-moc-extra

