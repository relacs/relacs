# AC_RELACS_CHECK_COMEDI() 
# - Provides --with(out)?-comedi options and performs header and link checks
# - Expects CLEAN_((LD|CPP)FLAGS|LIBS) vars to be set
# - Resets ((LD|CPP)FLAGS|LIBS) to CLEAN_\1 when done
# - Fills COMEDI_(LD|CPP)FLAGS and marks them for substitution

AC_DEFUN([AC_RELACS_CHECK_COMEDI], [
COMEDI_LDFLAGS=
COMEDI_CPPFLAGS=

LDFLAGS="$COMEDI_LDFLAGS $LDFLAGS"
CPPFLAGS="$COMEDI_CPPFLAGS $CPPFLAGS"

AC_ARG_WITH([comedi], [
  --with-comedi=DIR       set Comedi path
                          ("/lib" and "/include" is appended)
  --without-comedi        don't use Comedi, i.e. prevent auto-detection],
[
	# --without-comedi  -> $with_comedi = no
	# --with-comedi=no  -> $with_comedi = no
	# --with-comedi  -> $with_comedi = yes
	# --with-comedi=yes  -> $with_comedi = yes
	# --with-comedi=foo  -> $with_comedi = foo
	COMEDI_ERROR="No path given for option --with-comedi"
	AS_IF([test "x$with_comedi" != xyes -a "$xwith_comedi" != xcheck -a "x$with_comedi" != xno -a "x$with_comedi" != x],[
		COMEDI_LDFLAGS="-L${with_comedi}/lib $COMEDI_LDFLAGS"
		LDFLAGS="-L${with_comedi}/lib $LDFLAGS"
		COMEDI_CPPFLAGS="-I${with_comedi}/include $COMEDI_CPPFLAGS"
		CPPFLAGS="-I${with_comedi}/include $CPPFLAGS"
	        ],
              [test "x$with_comedi" = xyes],
		[AC_MSG_ERROR(${COMEDI_ERROR})],
              [])
],
[
	# no comedi argument given
	with_comedi=check
])

COMEDI_MISSING="Comedi not found in path ${with_comedi}."
AS_IF([test "x$with_comedi" != xno],
  [AC_CHECK_HEADERS([comedilib.h],
     [if test "x$with_comedi" != xcheck; then
        RELACS_COMEDI=$with_comedi
      else
        RELACS_COMEDI=yes
      fi
     ], 
     [if test "x$with_comedi" != xcheck; then
        AC_MSG_ERROR(${COMEDI_MISSING})
      fi
      RELACS_COMEDI=no
     ])
   AC_CHECK_LIB([comedi], [main],
     [if test "x$with_comedi" != xcheck; then
        RELACS_COMEDI=$with_comedi
      else
        RELACS_COMEDI=yes
      fi
     ], 
     [if test "x$with_comedi" != xcheck; then
        AC_MSG_ERROR(${COMEDI_MISSING})
      fi
      RELACS_COMEDI=no
     ])
  ],
  [ RELACS_COMEDI=no ] )

AC_SUBST(COMEDI_LDFLAGS)
AC_SUBST(COMEDI_CPPFLAGS)

# Restore
LDFLAGS=${CLEAN_LDFLAGS}
CPPFLAGS=${CLEAN_CPPFLAGS}
LIBS=${CLEAN_LIBS}
])

