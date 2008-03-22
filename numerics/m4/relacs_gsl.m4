# AC_RELACS_CHECK_GSL() 
# - Provides --with(out)?-gsl options and performs header and link checks
# - Expects CLEAN_((LD|CPP)FLAGS|LIBS) vars to be set
# - Resets ((LD|CPP)FLAGS|LIBS) to CLEAN_\1 when done
# - Fills GSL_(LD|CPP)FLAGS and marks them for substitution
# - Fills RELACS_GSL with (yes|no) for the summary
# - Extends DOXYGEN_PREDEF

AC_DEFUN([AC_RELACS_CHECK_GSL], [
GSL_LDFLAGS=
GSL_CPPFLAGS=

LDFLAGS="${GSL_LDFLAGS} ${LDFLAGS}"
CPPFLAGS="${GSL_CPPFLAGS} ${CPPFLAGS}"

AC_ARG_WITH(gsl, [
  --with-gsl@<:@=DIR@:>@        enable GSL
                          ("/lib" and "/include" is appended)
  --without-gsl           disable GSL, i.e. prevent auto-detection], [
	if test ${withval} = yes ; then
		RELACS_GSL=yes
	elif test ${withval} = no ; then
		RELACS_GSL=no
	elif test "x${withval}" = x ; then
		RELACS_GSL=yes
	else
		RELACS_GSL=yes
		GSL_LDFLAGS="-L${withval}/lib ${GSL_LDFLAGS}"
		GSL_CPPFLAGS="-I${withval}/include ${GSL_CPPFLAGS}"
		LDFLAGS="${GSL_LDFLAGS} ${LDFLAGS}"
		CPPFLAGS="${GSL_CPPFLAGS} ${CPPFLAGS}"
	fi

	if test ${RELACS_GSL} = yes ; then
		GSL_LDFLAGS="${GSL_LDFLAGS} -lgsl -lgslcblas"
		GSL_CPPFLAGS="${GSL_CPPFLAGS} -DGSL"

		LDFLAGS="${LDFLAGS} -lgsl -lgslcblas"
		CPPFLAGS="${CPPFLAGS} -DGSL"
	fi
	
], [
	RELACS_GSL=detect
])

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

	AC_CHECK_HEADERS(gsl/gsl_rng.h gsl/gsl_randist.h gsl/gsl_errno.h gsl/gsl_sf_gamma.h,, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	])
	AC_CHECK_LIB(gsl, main,, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	], -lgslcblas)
	AC_CHECK_LIB(gslcblas, main,, [
		if test ${RELACS_GSL} = yes ; then
			AC_MSG_ERROR(${GSL_MISSING})
		else			
			RELACS_GSL=error
		fi
	], -lgsl)

	if test ${RELACS_GSL} = error ; then
		AC_MSG_NOTICE(${GSL_DISABLED})
		RELACS_GSL=no
	elif test ${RELACS_GSL} = detect ; then
		AC_MSG_NOTICE(${GSL_ENABLED})
		DOXYGEN_PREDEF="${DOXYGEN_PREDEF} GSL"
		RELACS_GSL=yes
	fi
fi

AC_SUBST(GSL_LDFLAGS)
AC_SUBST(GSL_CPPFLAGS)
AC_SUBST(DOXYGEN_PREDEF)

# Restore
LDFLAGS=${CLEAN_LDFLAGS}
CPPFLAGS=${CLEAN_CPPFLAGS}
LIBS=${CLEAN_LIBS}
])

