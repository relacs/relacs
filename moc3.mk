MOC_CREATE = $(MOC) -o "$@" "$<"

MOC_COMPILE_1 = $(LIBTOOL) --tag=CXX \
	$(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=compile \
	$(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES)

MOC_COMPILE_2 = $(AM_CPPFLAGS) $(CPPFLAGS)

MOC_COMPILE_3 = $(AM_CXXFLAGS) $(CXXFLAGS) -c -o "$@" "$<"



clean-moc-extra:
	-rm -v -f *-moc.{cpp,o,lo}

clean: clean-moc-extra

