# RELACS_HEADER_RTAI() 
# - Provides --with(out)?-rtai options and performs header checks
# - Fills RTAI_CPPFLAGS and marks it for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_RTAI with the result of the tests

AC_DEFUN([RELACS_HEADER_RTAI], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}

# rtai flags:
RTAI_CPPFLAGS=

# read arguments:
AC_ARG_WITH([rtai],
[AS_HELP_STRING([--with-rtai=DIR],[set path to RTAI includes])
AS_HELP_STRING([--without-rtai],[don't use RTAI, i.e. prevent auto-detection])],
[
	# --without-rtai  -> $with_rtai = no
	# --with-rtai=no  -> $with_rtai = no
	# --with-rtai  -> $with_rtai = yes
	# --with-rtai=yes  -> $with_rtai = yes
	# --with-rtai=foo  -> $with_rtai = foo
	RTAI_ERROR="No path given for option --with-rtai"
	AS_IF([test "x$with_rtai" != xyes -a "$xwith_rtai" != xcheck -a "x$with_rtai" != xno -a "x$with_rtai" != x],
	        [RTAI_CPPFLAGS="-I${with_rtai}"
		 CPPFLAGS="${RTAI_CPPFLAGS} ${CPPFLAGS}"],
              [test "x$with_rtai" = xyes],
		[AC_MSG_ERROR(${RTAI_ERROR})],
              [])
],
[
	# no rtai argument given
	with_rtai=detect
])

# check rtai:
RTAI_MISSING="RTAI include files not found in path ${with_rtai}."
AS_IF([test "x$with_rtai" != xno],
  [AC_CHECK_HEADERS([rtai.h],
     [if test "x$with_rtai" != xdetect; then
        RELACS_RTAI=$with_rtai
      else
        RELACS_RTAI=yes
      fi
     ], 
     [if test "x$with_rtai" != xdetect; then
        AC_MSG_ERROR(${RTAI_MISSING})
      fi
      RELACS_RTAI=no
     ])
  ],
  [ RELACS_RTAI=no ] )

# publish:
AC_SUBST(RTAI_CPPFLAGS)

# restore:
CPPFLAGS=${SAVE_CPPFLAGS}

])

