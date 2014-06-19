# RELACS_LIB_PORTAUDIO() 
# - Provides --with(out)?-portaudio options and performs header and link checks
# - Fills (PORTAUDIO_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Extends DOXYGEN_PREDEF by HAVE_LIBPORTAUDIO
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_PORTAUDIO with the result of the tests

AC_DEFUN([RELACS_LIB_PORTAUDIO], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# portaudio flags:
PORTAUDIO_LDFLAGS=
PORTAUDIO_CPPFLAGS=
PORTAUDIO_LIBS=

# read arguments:
AC_ARG_WITH([portaudio],
[AS_HELP_STRING([--with-portaudio=DIR],[set portaudio path ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-portaudio],[don't use portaudio, i.e. prevent auto-detection])],
[
	# --without-portaudio  -> $with_portaudio = no
	# --with-portaudio=no  -> $with_portaudio = no
	# --with-portaudio  -> $with_portaudio = yes
	# --with-portaudio=yes  -> $with_portaudio = yes
	# --with-portaudio=foo  -> $with_portaudio = foo
	PORTAUDIO_ERROR="No path given for option --with-portaudio"
	AS_IF([test "x$with_portaudio" != xyes -a "$xwith_portaudio" != xcheck -a "x$with_portaudio" != xno -a "x$with_portaudio" != x],[
		PORTAUDIO_CPPFLAGS="-I${with_portaudio}/include"
		PORTAUDIO_LDFLAGS="-L${with_portaudio}/lib"
		CPPFLAGS="${PORTAUDIO_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${PORTAUDIO_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_portaudio" = xyes],
		[AC_MSG_ERROR(${PORTAUDIO_ERROR})],
              [])
],
[
	# no portaudio argument given
	with_portaudio=detect
])

# check portaudio:
PORTAUDIO_MISSING="Portaudio not found in path ${with_portaudio}."
AS_IF([test "x$with_portaudio" != xno],
  [SUCCESS=yes
   AC_CHECK_HEADERS([portaudio.h],, 
     [if test "x$with_portaudio" != xdetect; then
        AC_MSG_ERROR(${PORTAUDIO_MISSING})
      fi
      SUCCESS=no
     ])
   AC_CHECK_LIB([portaudio], [main],, 
     [if test "x$with_portaudio" != xdetect; then
        AC_MSG_ERROR(${PORTAUDIO_MISSING})
      fi
      SUCCESS=no
     ])
   AS_IF([test $SUCCESS = no],
	[RELACS_PORTAUDIO=no],
	[PORTAUDIO_LIBS="-lportaudio -lrt -lasound -ljack -lpthread"
	 if test "x$with_portaudio" != xdetect; then
        	RELACS_PORTAUDIO=$with_portaudio
      	 else
        	RELACS_PORTAUDIO=yes
      	 fi
	 DOXYGEN_PREDEF="${DOXYGEN_PREDEF} HAVE_LIBPORTAUDIO"
	])
  ],
  [ RELACS_PORTAUDIO=no ] )

# publish:
AC_SUBST(PORTAUDIO_LDFLAGS)
AC_SUBST(PORTAUDIO_CPPFLAGS)
AC_SUBST(PORTAUDIO_LIBS)
AC_SUBST(DOXYGEN_PREDEF)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

