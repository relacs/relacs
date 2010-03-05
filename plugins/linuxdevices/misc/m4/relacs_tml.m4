# RELACS_LIB_TML() 
# - Provides --with(out)?-tml options and performs header and link checks
# - Fills (TML_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_TML with the result of the tests

AC_DEFUN([RELACS_LIB_TML], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# tml flags:
TML_LDFLAGS=
TML_CPPFLAGS=
TML_LIBS=

# read arguments:
AC_ARG_WITH([tml],
[AS_HELP_STRING([--with-tml=DIR],[set TML path ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-tml],[don't use TML, i.e. prevent auto-detection])],
[
	# --without-tml  -> $with_tml = no
	# --with-tml=no  -> $with_tml = no
	# --with-tml  -> $with_tml = yes
	# --with-tml=yes  -> $with_tml = yes
	# --with-tml=foo  -> $with_tml = foo
	TML_ERROR="No path given for option --with-tml"
	AS_IF([test "x$with_tml" != xyes -a "$xwith_tml" != xcheck -a "x$with_tml" != xno -a "x$with_tml" != x],[
		TML_CPPFLAGS="-I${with_tml}/include"
		TML_LDFLAGS="-L${with_tml}/lib"
		CPPFLAGS="${TML_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${TML_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_tml" = xyes],
		[AC_MSG_ERROR(${TML_ERROR})],
              [])
],
[
	# no tml argument given
	with_tml=detect
])

# check tml:
TML_MISSING="TML not found in path ${with_tml}."
AS_IF([test "x$with_tml" != xno],
  [AC_CHECK_HEADERS([TML_lib.h],
     [if test "x$with_tml" != xdetect; then
        RELACS_TML=$with_tml
      else
        RELACS_TML=yes
      fi
     ], 
     [if test "x$with_tml" != xdetect; then
        AC_MSG_ERROR(${TML_MISSING})
      fi
      RELACS_TML=no
     ])
   AC_CHECK_LIB([TML_lib], [main],
     [TML_LIBS="-ltml"
      if test "x$with_tml" != xdetect; then
        RELACS_TML=$with_tml
      else
        RELACS_TML=yes
      fi
     ], 
     [if test "x$with_tml" != xdetect; then
        AC_MSG_ERROR(${TML_MISSING})
      fi
      RELACS_TML=no
     ])
  ],
  [ RELACS_TML=no ] )

# publish:
AC_SUBST(TML_LDFLAGS)
AC_SUBST(TML_CPPFLAGS)
AC_SUBST(TML_LIBS)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

