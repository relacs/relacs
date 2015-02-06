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
[AS_HELP_STRING([--with-rtai=DIR],[set path to RTAI base installation])
AS_HELP_STRING([--without-rtai],[don't use RTAI, i.e. prevent auto-detection])],
[
	# --without-rtai  -> $with_rtai = no
	# --with-rtai=no  -> $with_rtai = no
	# --with-rtai  -> $with_rtai = yes
	# --with-rtai=yes  -> $with_rtai = yes
	# --with-rtai=foo  -> $with_rtai = foo
	AS_IF([test "x$with_rtai" != xyes -a "x$with_rtai" != xno -a "x$with_rtai" != x],
	      [PATH="$PATH:${with_rtai}/bin/"; RTAI_CPPFLAGS="${with-rtai}"],
              [test "x$with_rtai" != xno],
		[PATH="$PATH:/usr/realtime/bin/"; RTAI_CPPFLAGS="/usr/realtime"
		 with_rtai=detect],
              [])
],
[
	# no rtai argument given
	PATH="$PATH:/usr/realtime/bin/"; 
        RTAI_CPPFLAGS="/usr/realtime"
	with_rtai=detect
])

# get CPP flags from rtai-config if possible:
RELACS_RTAI_VERSION=""
AS_IF( [ test "x$RTAI_CPPFLAGS" != "x" ],
       [ RELACS_RTAI_VERSION="`rtai-config --version`"
         AS_IF( [ test x"$RELACS_RTAI_VERSION" != x ], 
	 	[ RTAICPPFLAGS="`rtai-config -r --module-cflags`"
		  AS_IF( [ test x"$RTAICPPFLAGS" == x ], [ RTAICPPFLAGS="`rtai-config --module-cflags`" ] )
		  RTAI_CPPFLAGS="-I /usr/include $RTAICPPFLAGS" ],
		[ RTAI_CPPFLAGS="-I /usr/include -I /usr/realtime/include -D_FORTIFY_SOURCE=0 -ffast-math -mhard-float" ] ) ] )

# add CPU flags:
AS_IF( [ grep -q -w mmx /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -mmmx"] )
AS_IF( [ grep -q -w sse /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse"] )
AS_IF( [ grep -q -w sse2 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse2"] )
AS_IF( [ grep -q -w sse3 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse3"] )
AS_IF( [ grep -q -w ssse3 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -mssse3"] )
AS_IF( [ grep -q -w sse4 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4"] )
AS_IF( [ grep -q -w sse4_1 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4.1"] )
AS_IF( [ grep -q -w sse4_2 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4.2"] )

# set CFLAGS:
CPPFLAGS="${RTAI_CPPFLAGS} ${CPPFLAGS}"

# check rtai:
RTAI_MISSING="RTAI include files not found in path ${with_rtai}."
AS_IF([test "x$with_rtai" != xno],
  [AC_CHECK_HEADERS([rtai.h],
     [if test "x$with_rtai" != xdetect; then
        RELACS_RTAI="${with_rtai}"
      else
        RELACS_RTAI="yes"
      fi
     ], 
     [if test "x$with_rtai" != xdetect; then
        AC_MSG_ERROR(${RTAI_MISSING})
      fi
      RELACS_RTAI=no
     ])
  ],
  [ RELACS_RTAI=no ] )

# report rtai version and cpp flags:
AS_IF( [ test "x$RELACS_RTAI_VERSION" != x ], [ AC_MSG_NOTICE( "RTAI version ${RELACS_RTAI_VERSION}" ) ] )
AS_IF( [ test "x$RELACS_RTAI" != xno ], [ AC_MSG_NOTICE( "RTAI_CPPFLAGS=${RTAI_CPPFLAGS}" ) ] )

# publish:
AC_SUBST(RTAI_CPPFLAGS)

# restore:
CPPFLAGS=${SAVE_CPPFLAGS}

])

