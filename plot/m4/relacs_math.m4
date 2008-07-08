# RELACS_LIB_MATH() 
# - Checks for the standard C math libray (-lm)
# - Fills MATH_LIBS and marks it for substitution
# - Leaves LIBS untouched

AC_DEFUN([RELACS_LIB_MATH], [

# save flags:
SAVE_LIBS=${LIBS}

# math flags:
MATH_LIBS=

# check math:
MATH_MISSING="Standard math library (-lm) not found!"
AC_CHECK_LIB([m], [cos],, [AC_MSG_ERROR(${MATH_MISSING})] )

# publish:
MATH_LIBS="${LIBS}"
AC_SUBST(MATH_LIBS)

# restore:
LIBS=${SAVE_LIBS}
])

