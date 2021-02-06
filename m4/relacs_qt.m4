# RELACS_LIB_QT() 
# - Provides --with-qt(-(inc|lib))? options and performs header and link checks
# - Fills QTCORE_(LD|CPP)FLAGS and QTCORE_LIBS with values for the QtCore library and marks them for substitution
# - Fills QT_(LD|CPP)FLAGS and QT_LIBS with values for the QtCore, and QtGui library and marks them for substitution
# - Fills MOC and marks it for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched

AC_DEFUN([RELACS_LIB_QT], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# QT flags:
QTCORE_CPPFLAGS=
QTCORE_LDFLAGS=
QTCORE_LIBS=
QT_CPPFLAGS=
QT_LDFLAGS=
QT_LIBS=
RELACS_QT_VERSION="none"

# help message
QT_INSTALL="You either need to
- install a Qt development package (e.g. qtbase5-dev,
  on a Debian-based system enter 'sudo apt install qtbase5-dev'),
- or provide the path to the Qt include directory:
  e.g. './configure --with-qt-inc=/usr/lib/qt5/include'
- or provide the base path to the Qt installation:
  e.g. './configure --with-qt=/usr/lib/qt5'."

USE_QT5="true"
USE_QT4="true"
AC_ARG_WITH([qt5],
	[AS_HELP_STRING([--with-qt5],[use Qt5 libraries only])],
	[USE_QT4="false"],[])
AC_ARG_WITH([qt4],
	[AS_HELP_STRING([--with-qt4],[use Qt4 libraries only])],
	[USE_QT5="false"],[])

# get flags:
if ${USE_QT5} && test "x${PKG_CONFIG}" != "x" && ${PKG_CONFIG} --exists Qt5Gui ; then
    QTCORE_CPPFLAGS="`${PKG_CONFIG} --cflags Qt5Core`"
    QTCORE_LDFLAGS="`${PKG_CONFIG} --libs-only-L Qt5Core`"
    QTCORE_LIBS="`${PKG_CONFIG} --libs-only-l Qt5Core`"
    QT_CPPFLAGS="`${PKG_CONFIG} --cflags Qt5Core` `${PKG_CONFIG} --cflags Qt5Gui` `${PKG_CONFIG} --cflags Qt5Widgets`"
    QT_LDFLAGS="`${PKG_CONFIG} --libs-only-L Qt5Core` `${PKG_CONFIG} --libs-only-L Qt5Gui` `${PKG_CONFIG} --libs-only-L Qt5Widgets`"
    QT_LIBS="`${PKG_CONFIG} --libs-only-l Qt5Core` `${PKG_CONFIG} --libs-only-l Qt5Gui` `${PKG_CONFIG} --libs-only-l Qt5Widgets`"
elif ${USE_QT4} && test "x${PKG_CONFIG}" != "x" && ${PKG_CONFIG} --exists QtGui ; then
    QTCORE_CPPFLAGS="`${PKG_CONFIG} --cflags QtCore`"
    QTCORE_LDFLAGS="`${PKG_CONFIG} --libs-only-L QtCore`"
    QTCORE_LIBS="`${PKG_CONFIG} --libs-only-l QtCore`"
    QT_CPPFLAGS="`${PKG_CONFIG} --cflags QtCore` `${PKG_CONFIG} --cflags QtGui`"
    QT_LDFLAGS="`${PKG_CONFIG} --libs-only-L QtCore` `${PKG_CONFIG} --libs-only-L QtGui`"
    QT_LIBS="`${PKG_CONFIG} --libs-only-l QtCore` `${PKG_CONFIG} --libs-only-l QtGui`"
fi

# default flags:
if test "x${QTCORE_CPPFLAGS}" = x ; then
    QTCORE_CPPFLAGS="-DQT_SHARED -I/usr/include/qt5 -I/usr/include/qt5/QtCore"
fi
if test "x${QT_CPPFLAGS}" = x ; then
    QT_CPPFLAGS="-DQT_SHARED -I/usr/include/qt5 -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtGui"
fi
if test "x${QTCORE_LIBS}" = x ; then
    QTCORE_LIBS="-lQtCore"
fi
if test "x${QT_LIBS}" = x ; then
    QT_LIBS="-lQtGui -lQtCore"
fi

# read arguments:
WITH_QT="yes"
EXTRA_MOC_LOCATION=
AC_ARG_WITH([qt],
	[AS_HELP_STRING([--with-qt=DIR],
	           	[override QT path ("/lib" and "/include" is appended)])],
	[QT_ERROR="no path given for option --with-qt"
	if test ${withval} = no ; then
	   WITH_QT="no"
	elif test ${withval} != yes -a "x${withval}" != x ; then
	   	QTCORE_CPPFLAGS="-DQT_SHARED -I${withval}/include/qt5 -I${withval}/include/qt5/QtCore"
    		QT_CPPFLAGS="-DQT_SHARED -I${withval}/include/qt5 -I${withval}/include/qt5/QtCore -I${withval}/include/qt5/QtGui"
		QTCORE_LDFLAGS="-L${withval}/lib"
		QT_LDFLAGS="-L${withval}/lib"
		EXTRA_MOC_LOCATION="${withval}/bin"
	else
		AC_MSG_ERROR(${QT_ERROR})
	fi],
	[])

AC_ARG_WITH([qt-inc],
	[AS_HELP_STRING([--with-qt-inc=DIR],[override QT include path])],
	[QT_INC_ERROR="no path given for option --with-qt-inc"
	if test ${withval} != yes -a "x${withval}" != x ; then
	   	QTCORE_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore"
    		QT_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore -I${withval}/QtGui -I${withval}/QtCore"	
	else
		AC_MSG_ERROR(${QT_INC_ERROR})
	fi],
	[])

AC_ARG_WITH([qt-lib],
	[AS_HELP_STRING([--with-qt-lib=DIR],[override QT library path])],
	[QT_LIB_ERROR="no path given for option --with-qt-lib"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QTCORE_LDFLAGS="-L${withval}
		QT_LDFLAGS="-L${withval}
	else
		AC_MSG_ERROR(${QT_LIB_ERROR})
	fi],
	[])

if test $WITH_QT = "yes"; then

# update flags:
CPPFLAGS="${QT_CPPFLAGS} ${CPPFLAGS}"
LDFLAGS="${QT_LDFLAGS} ${LDFLAGS}"

QT_INC_MISSING="cannot find the header files for QT!
${QT_INSTALL}"
AC_CHECK_HEADERS(QWidget QThread,, AC_MSG_ERROR(${QT_INC_MISSING}))

QTCORE_LIB_MISSING="cannot find the QT core libraries!
${QT_INSTALL}"
AC_CHECK_LIB(QtCore, main,, AC_MSG_ERROR(${QTCORE_LIB_MISSING}), ${QTCORE_LDFLAGS})

QT_LIB_MISSING="cannot find the QT gui libraries!
${QT_INSTALL}"
AC_CHECK_LIB(QtGui, main,, AC_MSG_ERROR(${QT_LIB_MISSING}), ${QT_LDFLAGS})

QT_FAILED_VERSION="Failed to retrieve Qt version
${QT_INSTALL}"
AC_RUN_IFELSE([AC_LANG_PROGRAM([
#include <cstdio>
#include <QWidget>],
[std::printf("%s\n", QT_VERSION_STR)]
)], [RELACS_QT_VERSION=$(./conftest$EXEEXT)], AC_MSG_ERROR(${QT_FAILED_VESION}))

QT_WRONG_VESION="Qt version 4.0 or higher is required, you have another version!
${QT_INSTALL}"
AS_IF([test "${RELACS_QT_VERSION%%.*}" -lt 4],AC_MSG_ERROR(${QT_WRONG_VESION}))

fi
# WITH_QT

# publish:
AC_SUBST(QTCORE_CPPFLAGS)
AC_SUBST(QTCORE_LDFLAGS)
AC_SUBST(QTCORE_LIBS)
AC_SUBST(QT_CPPFLAGS)
AC_SUBST(QT_LDFLAGS)
AC_SUBST(QT_LIBS)
AC_SUBST(RELACS_QT_VERSION)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}


# moc:
MOC=moc

if test $WITH_QT = "yes"; then

FORCED_MOC=
AC_ARG_WITH([moc],
	[AS_HELP_STRING([--with-moc=CMD],[override moc command])],
	[QT_MOC_ERROR="No path given for option --with-moc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		FORCED_MOC=${withval}
	else
		AC_MSG_ERROR(${QT_MOC_ERROR})
	fi],
	[])

QT_MOC_VERSION="${RELACS_QT_VERSION%%.*}"
QT_MOC_VERSION_ERROR="$MOC is not for Qt ${QT_MOC_VERSION} (wrong version)!
   Please specify the full path to Moc:
   e.g. './configure --with-moc=/usr/lib/qt5/bin/moc'
   or provide the base path to the Qt installation:
   e.g. './configure --with-qt=/usr/lib/qt'."
if test "x${FORCED_MOC}" != x ; then
    # Moc command passed to configure
    MOC=${FORCED_MOC}
    if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
	    AC_MSG_ERROR(${QT_MOC_VERSION_ERROR})
    fi
else
    if test "x${EXTRA_MOC_LOCATION}" != x ; then
        # Moc from Qt directory
        MOC="${EXTRA_MOC_LOCATION}/moc"
        if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
            # Moc-Qt from path
            MOC=moc-QT
            if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
                # Moc from path
                MOC=moc
                if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
	                AC_MSG_ERROR(${QT_MOC_VERSION_ERROR})
                fi
            fi
        fi
    else
        # Moc-QT from path
        MOC=moc-QT
        if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
            # Moc from path
            MOC=moc
            if ! "${MOC}" -v 2>&1 | fgrep " ${QT_MOC_VERSION}" &>/dev/null ; then
                AC_MSG_ERROR(${QT_MOC_VERSION_ERROR})
            fi
        fi
    fi
fi

fi
# WITH_QT

AC_SUBST(MOC)

])
