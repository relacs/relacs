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
	AS_IF([test "x$with_rtai" != xyes -a "$xwith_rtai" != xcheck -a "x$with_rtai" != xno -a "x$with_rtai" != x],
	      [RTAI_CPPFLAGS="-I /usr/include `${with_rtai}/bin/rtai-config --module-cflags`"],
              [test "x$with_rtai" = xyes],
		[RTAI_CPPFLAGS="-I /usr/include `/usr/realtime/bin/rtai-config --module-cflags`"
		 with_rtai=detect],
              [])
],
[
	# no rtai argument given
        RTAI_CPPFLAGS="-I /usr/include `/usr/realtime/bin/rtai-config --module-cflags`"
	with_rtai=detect
])

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

