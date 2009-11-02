noinst_PROGRAMS = \
    ao_nonblocking \
    ao_underrun \
    ai_nonblocking \
    ai_maxrate \
    ai_multiboard \
    dio_write \
    checkao

noinst_HEADERS = examples.h

ao_nonblocking_LDFLAGS = $(COMEDI_LDFLAGS)
ao_nonblocking_LDADD = $(COMEDI_LIBS) $(MATH_LIBS)
ao_nonblocking_SOURCES = ao_nonblocking.c common.c

ao_underrun_LDFLAGS = $(COMEDI_LDFLAGS)
ao_underrun_LDADD = $(COMEDI_LIBS) $(MATH_LIBS)
ao_underrun_SOURCES = ao_underrun.c common.c

ai_nonblocking_LDFLAGS = $(COMEDI_LDFLAGS)
ai_nonblocking_LDADD = $(COMEDI_LIBS) $(MATH_LIBS)
ai_nonblocking_SOURCES = ai_nonblocking.c common.c

ai_maxrate_LDFLAGS = $(COMEDI_LDFLAGS)
ai_maxrate_LDADD = $(COMEDI_LIBS) $(MATH_LIBS)
ai_maxrate_SOURCES = ai_maxrate.c common.c

ai_multiboard_LDFLAGS = $(COMEDI_LDFLAGS)
ai_multiboard_LDADD = $(COMEDI_LIBS) $(MATH_LIBS)
ai_multiboard_SOURCES = ai_multiboard.c common.c

dio_write_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(srcdir)/../include
dio_write_LDFLAGS = $(COMEDI_LDFLAGS)
dio_write_LDADD = \
    $(COMEDI_LIBS) \
    $(top_builddir)/relacs/src/librelacs.la \
    ../src/libcomedicomedidigitalio.la
dio_write_SOURCES = dio_write.cc

checkao_CPPFLAGS = \
    -I$(top_srcdir)/daq/include \
    -I$(top_srcdir)/numerics/include \
    -I$(srcdir)/../include
checkao_LDFLAGS = \
    $(QT3_LDFLAGS)
checkao_LDADD = \
    $(top_builddir)/relacs/src/librelacs.la \
    ../src/libcomedicomedianaolginput.la \
    ../src/libcomedicomedianaolgoutput.la
checkao_SOURCES = checkao.cc
