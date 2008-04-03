# AC_RELACS_LINK_TESTS() 
# - Provides --disable-linktests option to disable static plugin link tests
# - Fills RELACS_LINKTESTS_ENABLED with (yes|no)
# - Creates an Automake conditional RELACS_LINKTESTS

AC_DEFUN([AC_RELACS_LINK_TESTS], [
RELACS_LINKTESTS_ENABLED=yes
AC_ARG_ENABLE(linktests, [
  --disable-linktests    disable static plugin link tests], [
	if test ${enableval} = no ; then
		RELACS_LINKTESTS_ENABLED=no
	fi
], [])

LINKTESTS_DISABLED="Link tests disabled"
LINKTESTS_ENABLED="Link tests enabled (configure with --disable-linktests to disable)"

if test ${RELACS_LINKTESTS_ENABLED} = yes ; then
	AC_MSG_NOTICE(${LINKTESTS_ENABLED})
else
	AC_MSG_NOTICE(${LINKTESTS_DISABLED})
fi

AM_CONDITIONAL([RELACS_LINKTESTS], [test ${RELACS_LINKTESTS_ENABLED} = yes])
])

