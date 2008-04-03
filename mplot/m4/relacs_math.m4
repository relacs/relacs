# AC_RELACS_CHECK_MATH() 
# - Checks for the standard C math libray (-lm)
# - Expects CLEAN_LIBS var to be set
# - Expects clean LIBS and sets it to CLEAN_\1 when done
# - Resets LIBS to CLEAN_\1 when done
# - Fills MATH_LIBS and marks it for substitution

AC_DEFUN([AC_RELACS_CHECK_MATH], [
MATH_LIBS=

MATH_MISSING="Standard math library (-lm) not found!"
AC_CHECK_LIB([m], [cos],, [AC_MSG_ERROR(${MATH_MISSING})] )

MATH_LIBS="${LIBS}"
AC_SUBST(MATH_LIBS)

# Restore
LIBS=${CLEAN_LIBS}
])

