noinst_PROGRAMS = \
    ao_nonblocking \
    ao_underrun \
    ai_nonblocking \
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
