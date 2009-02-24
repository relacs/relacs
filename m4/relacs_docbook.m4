# RELACS_DOCBOOK()
# - check whether docbook documentation should be generated
# - if yes, check for presence of xmlto program
# - set and substitute XMLTO to the path to the xmlto program
# - define RELACS_DOCBOOK_COND, RELACS_DOCBOOK_HTML_COND,
#   RELACS_DOCBOOK_PS_COND, RELACS_DOCBOOK_PDF_COND conditionals

AC_DEFUN([RELACS_DOCBOOK],[

# check whether the user manual should be generated:
AC_ARG_ENABLE([docbook],
    [AS_HELP_STRING([--disable-docbook],[do not generate the docbook manuals])],
    [AS_IF([test "x$enableval" = "xyes"],
        [RELACS_GENERATE_DOCBOOK="yes"],
        [test "x$enableval" = "xno"],
	[RELACS_GENERATE_DOCBOOK="no"],
	[AC_MSG_ERROR([bad value ${enableval} for --disable-docbook])])],
    [RELACS_GENERATE_DOCBOOK="yes"])

# check for presence of xmlto program:
if test "x$RELACS_GENERATE_DOCBOOK" == "xyes"; then
    AC_PATH_PROG( XMLTO, xmlto, no )
    if test "x$XMLTO" = "xno" ; then
        AC_MSG_WARN([xmlto not found, will not be able to rebuild documentation])
	RELACS_GENERATE_DOCBOOK="no"
	RELACS_GENERATE_DOCBOOK_HTML="no"
	RELACS_GENERATE_DOCBOOK_PS="no"
	RELACS_GENERATE_DOCBOOK_PDF="no"
    else
	RELACS_GENERATE_DOCBOOK_HTML="yes"
	RELACS_GENERATE_DOCBOOK_PS="no"
	RELACS_GENERATE_DOCBOOK_PDF="no"
    fi
else
    XMLTO="no"
    RELACS_GENERATE_DOCBOOK="no"
    RELACS_GENERATE_DOCBOOK_HTML="no"
    RELACS_GENERATE_DOCBOOK_PS="no"
    RELACS_GENERATE_DOCBOOK_PDF="no"
fi

# inform the user:
AC_MSG_NOTICE([generate docbook user manual: $RELACS_GENERATE_DOCBOOK ])

# define the conditionals:
AM_CONDITIONAL( [RELACS_DOCBOOK_COND], [test "$RELACS_GENERATE_DOCBOOK" != "no"] )
AM_CONDITIONAL( [RELACS_DOCBOOK_HTML_COND], [test "$RELACS_GENERATE_DOCBOOK_HTML" != "no"] )
AM_CONDITIONAL( [RELACS_DOCBOOK_PS_COND], [test "$RELACS_GENERATE_DOCBOOK_PS" != "no"] )
AM_CONDITIONAL( [RELACS_DOCBOOK_PDF_COND], [test "$RELACS_GENERATE_DOCBOOK_PDF" != "no"] )

])
