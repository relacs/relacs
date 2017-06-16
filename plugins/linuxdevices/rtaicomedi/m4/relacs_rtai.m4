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

USE_RTAI=yes

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
	      [PATH="$PATH:${with_rtai}/bin/"
 	       RTAI_CPPFLAGS="${with-rtai}"
	       USE_RTAI=yes],
              [test "x$with_rtai" != xno],
		[PATH="$PATH:/usr/realtime/bin/"
		 RTAI_CPPFLAGS="/usr/realtime"
		 USE_RTAI=yes; with_rtai=detect],
              [USE_RTAI=no])
],
[
	# no rtai argument given
	PATH="$PATH:/usr/realtime/bin/"; 
        RTAI_CPPFLAGS="/usr/realtime"
        USE_RTAI=yes
	with_rtai=detect
])

# check for presence of rtai-config script:
AS_IF( [ test $USE_RTAI == yes ],
       [ AC_CHECK_PROG( RTAI_CONFIG, [ rtai-config ], [ yes ], [ no ] )
	 AS_IF( [ test $RTAI_CONFIG == no ], 
                [ AC_MSG_NOTICE( "no rtai-config script found." )
                  USE_RTAI=no ] ) ] ) 

# support of -r option of rtai-config:
RUNNING_KERNEL=""
AS_IF( [ test $USE_RTAI == yes && bin/rtai-config -r --version &> /dev/null ],
       [ AC_MSG_NOTICE( "rtai-config script accepts -r options." )
         RUNNING_KERNEL="-r" ] )

# get RTAI version:
RELACS_RTAI_VERSION=""
AS_IF( [ test $USE_RTAI == yes ],
       [ RELACS_RTAI_VERSION="`rtai-config ${RUNNING_KERNEL} --version`"
         AS_IF( [ test "x$RELACS_RTAI_VERSION" == x ],
	 	[ AC_MSG_NOTICE( "could no retrieve RTAI version from rtai-config script" )
		  USE_RTAI=no ] ) 
       ] )
AS_IF( [ test "x$RELACS_RTAI_VERSION" != x ], [ AC_MSG_NOTICE( "RTAI version ${RELACS_RTAI_VERSION}" ) ] )

# check running kernel:
AS_IF( [ test $USE_RTAI == yes && ! grep -q IPIPE /boot/config-`uname -r` ],
       [ AC_MSG_NOTICE( "the running kernel is not an RTAI patched kernel" ); USE_RTAI=no ] )
AS_IF( [ test $USE_RTAI == yes && ! expr match `uname -r` `rtai-config ${RUNNING_KERNEL} --linux-version` ],
       [ AC_MSG_NOTICE( "the version of running kernel does not match the one reported by rtai-config" ); USE_RTAI=no ] )

# get CPP flags from rtai-config if possible:
AS_IF( [ test $USE_RTAI == yes ],
       [ RTAI_CPPFLAGS="-I /usr/include `rtai-config ${RUNNING_KERNEL} --module-cflags`"
       	 RTAI_CPPFLAGS=${RTAI_CPPFLAGS/-I. /}
	 AS_IF( [ test "x$RTAI_CPPFLAGS" == x ], 
	 	[ RTAI_CPPFLAGS="-I /usr/include -I /usr/realtime/include -D_FORTIFY_SOURCE=0 -ffast-math -mhard-float" ] ) 
       ] )

# add CPU specific CPU flags:
AS_IF( [ grep -q -w mmx /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -mmmx"] )
AS_IF( [ grep -q -w sse /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse"] )
AS_IF( [ grep -q -w sse2 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse2"] )
AS_IF( [ grep -q -w sse3 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse3"] )
AS_IF( [ grep -q -w ssse3 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -mssse3"] )
AS_IF( [ grep -q -w sse4 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4"] )
AS_IF( [ grep -q -w sse4_1 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4.1"] )
AS_IF( [ grep -q -w sse4_2 /proc/cpuinfo ], [ RTAI_CPPFLAGS="$RTAI_CPPFLAGS -msse4.2"] )

AS_IF( [ test $USE_RTAI == yes ], [ AC_MSG_NOTICE( "RTAI_CPPFLAGS=${RTAI_CPPFLAGS}" ) ] )

# set CFLAGS:
CPPFLAGS="${RTAI_CPPFLAGS} ${CPPFLAGS}"

# check rtai headers:
AS_IF( [ test $USE_RTAI == yes ],
       [ AC_CHECK_HEADERS( [rtai.h],
       	 		   [ AS_IF( [ test "x${with_rtai}" != xdetect ],
        		     	    [ RELACS_RTAI="${with_rtai}" ],
        			    [ RELACS_RTAI="yes" ] ) ], 
     			   [ AC_MSG_NOTICE( "RTAI include files not found in path ${with_rtai}." )
      			     RELACS_RTAI=no
			     USE_RTAI=no ] ) ],
	[ RELACS_RTAI=no ] )

# publish:
AC_SUBST(RTAI_CPPFLAGS)

# restore:
CPPFLAGS=${SAVE_CPPFLAGS}

])

