# RELACS_LIB_COMEDI() 
# - Provides --with(out)?-comedi options and performs header and link checks
# - Fills (COMEDI_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_COMEDI with the result of the tests

AC_DEFUN([RELACS_LIB_COMEDI], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# comedi flags:
COMEDI_LDFLAGS=
COMEDI_CPPFLAGS=
COMEDI_LIBS=

# read arguments:
AC_ARG_WITH([comedi],
[AS_HELP_STRING([--with-comedi=DIR],[set Comedi path ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-comedi],[don't use Comedi, i.e. prevent auto-detection])],
[
	# --without-comedi  -> $with_comedi = no
	# --with-comedi=no  -> $with_comedi = no
	# --with-comedi  -> $with_comedi = yes
	# --with-comedi=yes  -> $with_comedi = yes
	# --with-comedi=foo  -> $with_comedi = foo
	COMEDI_ERROR="No path given for option --with-comedi"
	AS_IF([test "x$with_comedi" != xyes -a "$xwith_comedi" != xcheck -a "x$with_comedi" != xno -a "x$with_comedi" != x],[
		COMEDI_CPPFLAGS="-I${with_comedi}/include"
		COMEDI_LDFLAGS="-L${with_comedi}/lib"
		CPPFLAGS="${COMEDI_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${COMEDI_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_comedi" = xyes],
		[AC_MSG_ERROR(${COMEDI_ERROR})],
              [])
],
[
	# no comedi argument given
	with_comedi=detect
])

# check comedi:
COMEDI_MISSING="Comedi not found in path ${with_comedi}."
AS_IF([test "x$with_comedi" != xno],
  [AC_CHECK_HEADERS([comedilib.h],
     [if test "x$with_comedi" != xdetect; then
        RELACS_COMEDI=$with_comedi
      else
        RELACS_COMEDI=yes
      fi
     ], 
     [if test "x$with_comedi" != xdetect; then
        AC_MSG_ERROR(${COMEDI_MISSING})
      fi
      RELACS_COMEDI=no
     ])
   AC_CHECK_LIB([comedi], [main],
     [COMEDI_LIBS="-lcomedi"
      if test "x$with_comedi" != xdetect; then
        RELACS_COMEDI=$with_comedi
      else
        RELACS_COMEDI=yes
      fi
     ], 
     [if test "x$with_comedi" != xdetect; then
        AC_MSG_ERROR(${COMEDI_MISSING})
      fi
      RELACS_COMEDI=no
     ])
  ],
  [ RELACS_COMEDI=no ] )

# publish:
AC_SUBST(COMEDI_LDFLAGS)
AC_SUBST(COMEDI_CPPFLAGS)
AC_SUBST(COMEDI_LIBS)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

