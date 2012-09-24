# RELACS_LIB_OPENCV() 
# - Provides --with(out)?-opencv options and performs header and link checks
# - Fills (OPENCV_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_OPENCV with the result of the tests

AC_DEFUN([RELACS_LIB_OPENCV], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# opencv flags:
OPENCV_LDFLAGS=
OPENCV_CPPFLAGS=
OPENCV_LIBS=

# read arguments:
AC_ARG_WITH([opencv],
[AS_HELP_STRING([--with-opencv=DIR],[set OpenCV path ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-opencv],[don't use OpenCV, i.e. prevent auto-detection])],
[
	# --without-opencv  -> $with_opencv = no
	# --with-opencv=no  -> $with_opencv = no
	# --with-opencv  -> $with_opencv = yes
	# --with-opencv=yes  -> $with_opencv = yes
	# --with-opencv=foo  -> $with_opencv = foo
	OPENCV_ERROR="No path given for option --with-opencv"
	AS_IF([test "x$with_opencv" != xyes -a "$xwith_opencv" != xcheck -a "x$with_opencv" != xno -a "x$with_opencv" != x],[
		OPENCV_CPPFLAGS="-I${with_opencv}/include"
		OPENCV_LDFLAGS="-L${with_opencv}/lib"
		CPPFLAGS="${OPENCV_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${OPENCV_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_opencv" = xyes],
		[AC_MSG_ERROR(${OPENCV_ERROR})],
              [])
],
[
	# no opencv argument given
	with_opencv=detect
])

# set standard pathes:
AS_IF([test "x$with_opencv" = xdetect],
      [ OPENCV_CPPFLAGS="-I/usr/include/opencv"
	CPPFLAGS="${OPENCV_CPPFLAGS} ${CPPFLAGS}"
      ])

# check opencv:
OPENCV_MISSING="OpenCV not found in path ${with_opencv}."
AS_IF([test "x$with_opencv" != xno],
  [SUCCESS=yes
   AC_CHECK_HEADERS([cv.h highgui.h],
     [if test "x$with_opencv" != xdetect; then
        RELACS_OPENCV=$with_opencv
      else
        RELACS_OPENCV=yes
      fi
     ], 
     [if test "x$with_opencv" != xdetect; then
        AC_MSG_ERROR(${OPENCV_MISSING})
      fi
      SUCCESS=no
     ])
   AC_CHECK_LIB(cv, [main],
     [OPENCV_LIBS="-lcv -lhighgui -lcvaux"
      if test "x$with_opencv" != xdetect; then
        RELACS_OPENCV=$with_opencv
      else
        RELACS_OPENCV=yes
      fi
     ], 
     [if test "x$with_opencv" != xdetect; then
        AC_MSG_ERROR(${OPENCV_MISSING})
      fi
      SUCCESS=no
     ], [-lhighgui -lcvaux -lm])
   AS_IF([test $SUCCESS = no],[RELACS_OPENCV=no])
  ],
  [ RELACS_OPENCV=no ] )

# publish:
AC_SUBST(OPENCV_LDFLAGS)
AC_SUBST(OPENCV_CPPFLAGS)
AC_SUBST(OPENCV_LIBS)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

# for opencv version 2 (from Michael Hanke):
# -   AC_CHECK_LIB(cv, [main],
# -     [OPENCV_LIBS="-lcv -lhighgui -lcvaux"
# +   AC_CHECK_LIB(opencv_core, [main],
# +     [OPENCV_LIBS="-lopencv_core -lopencv_highgui -lopencv_calib3d -lopencv_imgproc"
# -     ], [-lhighgui -lcvaux -lm])
# +     ], [-lopencv_highgui -lm])

