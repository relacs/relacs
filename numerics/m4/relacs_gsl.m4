# RELACS_LIB_GSL() 
# - Provides --with(out)?-gsl options and performs header and link checks
# - Checks for standard math library first
# - Fills GSL_(LD|CPP)FLAGS and GSL_LIBS and marks them for substitution
# - Adds HAVE_LIBGSL to the C preprocessor defines
# - Fills RELACS_GSL with (yes|no) for the summary
# - Extends DOXYGEN_PREDEF by HAVE_LIBGSL
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched

AC_DEFUN([RELACS_LIB_GSL], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# GSL flags:
GSL_CPPFLAGS=
GSL_LDFLAGS=
GSL_LIBS=

# read arguments:
AC_ARG_WITH([gsl],
[AS_HELP_STRING([--with-gsl@<:@=DIR@:>@],
		[enable GSL ("/lib" and "/include" is appended)])
AS_HELP_STRING([--without-gsl],	[disable GSL, i.e. prevent auto-detection])],
 	[
	if test ${withval} = yes ; then
		RELACS_GSL=yes
	elif test ${withval} = no ; then
		RELACS_GSL=no
	elif test "x${withval}" = x ; then
		RELACS_GSL=yes
	else
		RELACS_GSL=yes
		GSL_CPPFLAGS="-I${withval}/include"
		GSL_LDFLAGS="-L${withval}/lib"
		CPPFLAGS="${GSL_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${GSL_LDFLAGS} ${LDFLAGS}"
	fi],
	[RELACS_GSL=detect])

GSL_DISABLED="GSL disabled (configure with --with-gsl@<:@=DIR@:>@ to enable)"
GSL_ENABLED="GSL enabled"

if test ${RELACS_GSL} = yes ; then
	AC_MSG_NOTICE(${GSL_ENABLED})
elif test ${RELACS_GSL} = no ; then
	AC_MSG_NOTICE(${GSL_DISABLED})
fi

if test ! ${RELACS_GSL} = no ; then
	GSL_MISSING="Please install the GNU Scientific Library (GSL).
   On a Debian-based system enter 'sudo apt-get install libgsl0-dev'."
	GSL_MATH_MISSING="Standard math library (-lm) not found!"

	AC_CHECK_HEADERS(gsl/gsl_rng.h gsl/gsl_randist.h gsl/gsl_errno.h gsl/gsl_sf_gamma.h,, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	])
# check for GSL library as recommended by
# http://www.gnu.org/software/gsl/manual/html_node/Autoconf-Macros.html
	AC_CHECK_LIB([m], [cos],, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MATH_MISSING})
		else			
			RELACS_GSL=error
		fi
        ])
	AC_CHECK_LIB([gslcblas], [cblas_dgemm],, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	])
	AC_CHECK_LIB([gsl], [gsl_blas_dgemm],, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	])

	GSL_LIBS="${LIBS}"
	if test ${RELACS_GSL} = error ; then
		AC_MSG_NOTICE(${GSL_DISABLED})
		RELACS_GSL=no
		GSL_CPPFLAGS=""
		GSL_LDFLAGS=""
		GSL_LIBS=""
	elif test ${RELACS_GSL} = detect ; then
		AC_MSG_NOTICE(${GSL_ENABLED})
		RELACS_GSL=yes
	fi
	if test ${RELACS_GSL} = yes ; then
		DOXYGEN_PREDEF="${DOXYGEN_PREDEF} HAVE_LIBGSL"
	fi
fi

# publish:
AC_SUBST(GSL_CPPFLAGS)
AC_SUBST(GSL_LDFLAGS)
AC_SUBST(GSL_LIBS)
AC_SUBST(DOXYGEN_PREDEF)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

