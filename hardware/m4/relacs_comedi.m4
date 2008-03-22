# AC_RELACS_CHECK_COMEDI() 
# - Provides --with(out)?-comedi options and performs header and link checks
# - Expects CLEAN_((LD|CPP)FLAGS|LIBS) vars to be set
# - Resets ((LD|CPP)FLAGS|LIBS) to CLEAN_\1 when done
# - Fills COMEDI_(LD|CPP)FLAGS and marks them for substitution

AC_DEFUN([AC_RELACS_CHECK_COMEDI], [
COMEDI_LDFLAGS=
COMEDI_CPPFLAGS=

LDFLAGS="${COMEDI_LDFLAGS} ${LDFLAGS}"
CPPFLAGS="${COMEDI_CPPFLAGS} ${CPPFLAGS}"

AC_ARG_WITH(comedi, [
  --with-comedi=DIR       override Comedi path
                          ("/lib" and "/include" is appended)], [
	COMEDI_ERROR="No path given for option --with-comedi"
	if test ${withval} != yes -a "x${withval}" != x ; then
		COMEDI_LDFLAGS="-L${withval} ${COMEDI_LDFLAGS}"
		LDFLAGS="-L${withval} ${LDFLAGS}"
		COMEDI_CPPFLAGS="-I${withval} ${COMEDI_CPPFLAGS}"
		CPPFLAGS="-I${withval} ${CPPFLAGS}"
	else
		AC_MSG_ERROR(${COMEDI_ERROR})
	fi
], [])
                          
COMEDI_MISSING="Please install Comedi.
   On a Debian-based system enter 'sudo apt-get install libcomedi-dev'."
AC_CHECK_HEADERS(comedilib.h,, AC_MSG_ERROR(${COMEDI_MISSING}))
AC_CHECK_LIB(comedi, main,, AC_MSG_ERROR(${COMEDI_MISSING}))

AC_SUBST(COMEDI_LDFLAGS)
AC_SUBST(COMEDI_CPPFLAGS)

# Restore
LDFLAGS=${CLEAN_LDFLAGS}
CPPFLAGS=${CLEAN_CPPFLAGS}
LIBS=${CLEAN_LIBS}
])

