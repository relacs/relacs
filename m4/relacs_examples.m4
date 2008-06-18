AC_DEFUN([RELACS_EXAMPLES],[

# check whether the examples should be compiled:
AC_ARG_ENABLE([examples],
    [AS_HELP_STRING([--enable-examples],[compile the example programs])],
    [AS_IF([test "x$enableval" = "xyes"],
        [RELACS_COMPILE_EXAMPLES="yes"],
        [test "x$enableval" = "xno"],
	[RELACS_COMPILE_EXAMPLES="no"],
	[AC_MSG_ERROR([bad value ${enableval} for --enable-examples])])],
    [RELACS_COMPILE_EXAMPLES="no"])

# inform the user:
AC_MSG_NOTICE([compile example programs: $RELACS_COMPILE_EXAMPLES ])

# define the conditional:
AM_CONDITIONAL([RELACS_EXAMPLES_COND], [test x$RELACS_COMPILE_EXAMPLES = xyes])

])

