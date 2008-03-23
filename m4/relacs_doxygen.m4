# AC_RELACS_CHECK_DOXYGEN() 
# - Checks for doxygen, graphiz and latex commands
# - Fills DOXYGEN_ENABLED with (yes|no) for the summary
# - Creates an Automake conditional RELACS_CREATE_DOC

AC_DEFUN([AC_RELACS_CHECK_DOXYGEN], [
DOXYGEN_ENABLED=no
## Doxygen
AC_CHECK_PROG(DOXY_CHECK, doxygen, found, missing)
if test ${DOXY_CHECK} != missing ; then
	## Graphviz
	AC_CHECK_PROG(GRAPHVIZ_CHECK, dot, found, missing)
	if test ${GRAPHVIZ_CHECK} != missing ; then
		## LaTeX
		AC_CHECK_PROG(LATEX_CHECK, latex, found, missing)
		if test ${LATEX_CHECK} != missing ; then
			DOXYGEN_ENABLED=yes
		fi
	fi
fi

AM_CONDITIONAL([RELACS_CREATE_DOC], [test ${DOXYGEN_ENABLED} = yes])
])

