# AC_RELACS_CHECK_QT3() 
# - Provides --with-qt3(-(inc|lib))? options and performs header and link checks
# - Expects CLEAN_((LD|CPP)FLAGS|LIBS) vars to be set
# - Resets ((LD|CPP)FLAGS|LIBS) to CLEAN_\1 when done
# - Fills QT3_(LD|CPP)FLAGS and marks them for substitution
# - Fills MOC and marks it for substitution

AC_DEFUN([AC_RELACS_CHECK_QT3], [
QT3_LDFLAGS=`pkg-config --libs qt-mt`
if test "x${QT3_LDFLAGS}" = x ; then
    QT3_LDFLAGS=-lqt-mt
fi
QT3_CPPFLAGS=`pkg-config --cflags qt-mt`
if test "x${QT3_CPPFLAGS}" = x ; then
    QT3_CPPFLAGS=-DQT_THREAD_SUPPORT
fi

LDFLAGS="${QT3_LDFLAGS} ${LDFLAGS}"
CPPFLAGS="${QT3_CPPFLAGS} ${CPPFLAGS}"

EXTRA_MOC_LOCATION=
AC_ARG_WITH(qt3, [
Extra Options:
  --with-qt3=DIR          override Qt3 path
                          ("/lib" and "/include" is appended], [
	QT3_ERROR="No path given for option --with-qt3"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_LDFLAGS="-L${withval} ${QT3_LDFLAGS}"
		LDFLAGS="-L${withval} ${LDFLAGS}"
		QT3_CPPFLAGS="-I${withval} ${QT3_CPPFLAGS}"
		CPPFLAGS="-I${withval} ${CPPFLAGS}"
		EXTRA_MOC_LOCATION="${withval}/bin"
	else
		AC_MSG_ERROR(${QT3_ERROR})
	fi
], [])

AC_ARG_WITH(qt3-inc, [  --with-qt3-inc=DIR      override Qt3 include path], [
	QT3_INC_ERROR="No path given for option --with-qt3-inc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_CPPFLAGS="-I${withval} ${QT3_CPPFLAGS}"
		CPPFLAGS="-I${withval} ${CPPFLAGS}"
	else
		AC_MSG_ERROR(${QT3_INC_ERROR})
	fi
], [])

AC_ARG_WITH(qt3-lib, [  --with-qt3-lib=DIR      override Qt3 library path], [
	QT3_LIB_ERROR="No path given for option --with-qt3-lib"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT3_LDFLAGS="-L${withval} ${QT3_LDFLAGS}"
		LDFLAGS="-L${withval} ${LDFLAGS}"
	else
		AC_MSG_ERROR(${QT3_LIB_ERROR})
	fi
], [])

QT3_MISSING="Please install Qt3.
   On a Debian-based system enter 'sudo apt-get install libqt3-mt-dev'."
AC_CHECK_HEADERS(qvbox.h qconfig.h,, AC_MSG_ERROR(${QT3_MISSING}))
AC_CHECK_LIB(qt-mt, main,, AC_MSG_ERROR(${QT3_MISSING}), ${QT3_LDFLAGS})

QT3_WRONG_VESION="Qt3 is required, you have another version."
AC_COMPILE_IFELSE([
#include <qvbox.h>
#if !defined(QT_VERSION) || (QT_VERSION < 0x030300) || (QT_VERSION >= 0x040000)
# error Wrong Qt version
#endif
],,AC_MSG_ERROR(${QT3_WRONG_VESION}))

AC_SUBST(QT3_LDFLAGS)
AC_SUBST(QT3_CPPFLAGS)

# Restore
LDFLAGS=${CLEAN_LDFLAGS}
CPPFLAGS=${CLEAN_CPPFLAGS}
LIBS=${CLEAN_LIBS}



FORCED_MOC=
AC_ARG_WITH(moc, [  --with-moc=CMD          override moc command], [
	QT3_MOC_ERROR="No path given for option --with-moc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		FORCED_MOC=${withval}
	else
		AC_MSG_ERROR(${QT3_MOC_ERROR})
	fi
], [])

QT3_MOC_VERSION_ERROR="\$MOC is not pointing to Moc of Qt3"
if test "x${FORCED_MOC}" != x ; then
    # Moc command passed to configure
    MOC=${FORCED_MOC}
    if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
	    AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
    fi
else
    if test "x${EXTRA_MOC_LOCATION}" != x ; then
        # Moc from Qt3 directory
        MOC="${EXTRA_MOC_LOCATION}/moc"
        if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
            # Moc-qt3 from path
            MOC=moc-qt3
            if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
                # Moc from path
                MOC=moc
                if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
	                AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
                fi
            fi
        fi
    else
        # Moc-qt3 from path
        MOC=moc-qt3
        if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
            # Moc from path
            MOC=moc
            if ! "${MOC}" -v 2>&1 | grep 'Qt 3.3' &>/dev/null ; then
                AC_MSG_ERROR(${QT3_MOC_VERSION_ERROR})
            fi
        fi
    fi
fi

AC_SUBST(MOC)
])

