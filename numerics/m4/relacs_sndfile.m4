# RELACS_LIB_SNDFILE() 
# - Provides --with(out)?-sndfile options and performs header and link checks
# - Fills (SNDFILE_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Extends DOXYGEN_PREDEF by HAVE_LIBSNDFILE
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_SNDFILE with the result of the tests

AC_DEFUN([RELACS_LIB_SNDFILE], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# sndfile flags:
SNDFILE_LDFLAGS=
SNDFILE_CPPFLAGS=
SNDFILE_LIBS=

# read arguments:
AC_ARG_WITH([sndfile],
[AS_HELP_STRING([--with-sndfile=DIR],[set sndfile path ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-sndfile],[don't use sndfile, i.e. prevent auto-detection])],
[
	# --without-sndfile  -> $with_sndfile = no
	# --with-sndfile=no  -> $with_sndfile = no
	# --with-sndfile  -> $with_sndfile = yes
	# --with-sndfile=yes  -> $with_sndfile = yes
	# --with-sndfile=foo  -> $with_sndfile = foo
	SNDFILE_ERROR="No path given for option --with-sndfile"
	AS_IF([test "x$with_sndfile" != xyes -a "$xwith_sndfile" != xcheck -a "x$with_sndfile" != xno -a "x$with_sndfile" != x],[
		SNDFILE_CPPFLAGS="-I${with_sndfile}/include"
		SNDFILE_LDFLAGS="-L${with_sndfile}/lib"
		CPPFLAGS="${SNDFILE_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${SNDFILE_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_sndfile" = xyes],
		[AC_MSG_ERROR(${SNDFILE_ERROR})],
              [])
],
[
	# no sndfile argument given
	with_sndfile=detect
])

# check sndfile:
SNDFILE_MISSING="Sndfile not found in path ${with_sndfile}."
AS_IF([test "x$with_sndfile" != xno],
  [SUCCESS=yes
   AC_CHECK_HEADERS([sndfile.h],, 
     [if test "x$with_sndfile" != xdetect; then
        AC_MSG_ERROR(${SNDFILE_MISSING})
      fi
      SUCCESS=no
     ])
   AC_CHECK_LIB([sndfile], [main],, 
     [if test "x$with_sndfile" != xdetect; then
        AC_MSG_ERROR(${SNDFILE_MISSING})
      fi
      SUCCESS=no
     ])
   AS_IF([test $SUCCESS = no],
	[RELACS_SNDFILE=no],
	[SNDFILE_LIBS="-lsndfile"
	 if test "x$with_sndfile" != xdetect; then
        	RELACS_SNDFILE=$with_sndfile
      	 else
        	RELACS_SNDFILE=yes
      	 fi
	 DOXYGEN_PREDEF="${DOXYGEN_PREDEF} HAVE_LIBGSL"
	])
  ],
  [ RELACS_SNDFILE=no ] )

# publish:
AC_SUBST(SNDFILE_LDFLAGS)
AC_SUBST(SNDFILE_CPPFLAGS)
AC_SUBST(SNDFILE_LIBS)
AC_SUBST(DOXYGEN_PREDEF)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

