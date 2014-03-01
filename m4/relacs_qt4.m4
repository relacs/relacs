# RELACS_LIB_QT4() 
# - Provides --with-qt4(-(inc|lib))? options and performs header and link checks
# - Fills QT4CORE_(LD|CPP)FLAGS and QT4CORE_LIBS with values for the QtCore library and marks them for substitution
# - Fills QT4_(LD|CPP)FLAGS and QT4_LIBS with values for the QtCore and QtGui library and marks them for substitution
# - Fills MOC and marks it for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched

AC_DEFUN([RELACS_LIB_QT4], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# QT4 flags:
QT4CORE_CPPFLAGS=
QT4CORE_LDFLAGS=
QT4CORE_LIBS=
QT4_CPPFLAGS=
QT4_LDFLAGS=
QT4_LIBS=

# get flags:
if test "x${PKG_CONFIG}" != "x" && ${PKG_CONFIG} --exists QtGui ; then
    QT4CORE_CPPFLAGS="`${PKG_CONFIG} --cflags QtCore`"
    QT4CORE_LDFLAGS="`${PKG_CONFIG} --libs-only-L QtCore`"
    QT4CORE_LIBS="`${PKG_CONFIG} --libs-only-l QtCore`"
    QT4_CPPFLAGS="`${PKG_CONFIG} --cflags QtCore` `${PKG_CONFIG} --cflags QtGui`"
    QT4_LDFLAGS="`${PKG_CONFIG} --libs-only-L QtCore` `${PKG_CONFIG} --libs-only-L QtGui`"
    QT4_LIBS="`${PKG_CONFIG} --libs-only-l QtCore` `${PKG_CONFIG} --libs-only-l QtGui`"
fi

# default flags:
if test "x${QT4CORE_CPPFLAGS}" = x ; then
    QT4CORE_CPPFLAGS="-DQT_SHARED -I/usr/include/qt4 -I/usr/include/qt4/QtCore"
QtGui"
fi
if test "x${QT4_CPPFLAGS}" = x ; then
    QT4_CPPFLAGS="-DQT_SHARED -I/usr/include/qt4 -I/usr/include/qt4/QtCore -I/usr/include/qt4/fi
fi
if test "x${QT4CORE_LIBS}" = x ; then
    QT4CORE_LIBS="-lQtCore"
fi
if test "x${QT4_LIBS}" = x ; then
    QT4_LIBS="-lQtGui -lQtCore"
fi

# read arguments:
EXTRA_MOC_LOCATION=
AC_ARG_WITH([qt4],
	[AS_HELP_STRING([--with-qt4=DIR],
	           	[override Qt4 path ("/lib" and "/include" is appended)])],
	[QT4_ERROR="no path given for option --with-qt4"
	if test ${withval} != yes -a "x${withval}" != x ; then
	   	QT4CORE_CPPFLAGS="-DQT_SHARED -I${withval}/include/qt4 -I${withval}/include/qt4/QtCore"
    		QT4_CPPFLAGS="-DQT_SHARED -I${withval}/include/qt4 -I${withval}/include/qt4/QtCore -I${withval}/include/qt4/QtGui"	
		QT4CORE_LDFLAGS="-L${withval}/lib"
		QT4_LDFLAGS="-L${withval}/lib"
		EXTRA_MOC_LOCATION="${withval}/bin"
	else
		AC_MSG_ERROR(${QT4_ERROR})
	fi],
	[])

AC_ARG_WITH([qt4-inc],
	[AS_HELP_STRING([--with-qt4-inc=DIR],[override Qt4 include path])],
	[QT4_INC_ERROR="no path given for option --with-qt4-inc"
	if test ${withval} != yes -a "x${withval}" != x ; then
	   	QT4CORE_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore"
    		QT4_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore -I${withval}/QtGui"	
	else
		AC_MSG_ERROR(${QT4_INC_ERROR})
	fi],
	[])

AC_ARG_WITH([qt4-lib],
	[AS_HELP_STRING([--with-qt4-lib=DIR],[override Qt4 library path])],
	[QT4_LIB_ERROR="no path given for option --with-qt4-lib"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QT4CORE_LDFLAGS="-L${withval}
		QT4_LDFLAGS="-L${withval}
	else
		AC_MSG_ERROR(${QT4_LIB_ERROR})
	fi],
	[])

# update flags:
CPPFLAGS="${QT4_CPPFLAGS} ${CPPFLAGS}"
LDFLAGS="${QT4_LDFLAGS} ${LDFLAGS}"

QT4_INC_MISSING="cannot find the header files for Qt4!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 include directory:
  e.g. './configure --with-qt4-inc=/usr/lib/qt4/include'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_HEADERS(QWidget QThread,, AC_MSG_ERROR(${QT4_INC_MISSING}))

QT4CORE_LIB_MISSING="cannot find the Qt4 core libraries!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 libraries:
  e.g. './configure --with-qt4-lib=/usr/lib/qt4/lib'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_LIB(QtCore, main,, AC_MSG_ERROR(${QT4CORE_LIB_MISSING}), ${QT4CORE_LDFLAGS})

QT4_LIB_MISSING="cannot find the Qt4 gui libraries!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 libraries:
  e.g. './configure --with-qt4-lib=/usr/lib/qt4/lib'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_LIB(QtGui, main,, AC_MSG_ERROR(${QT4_LIB_MISSING}), ${QT4_LDFLAGS})

QT4_WRONG_VESION="Qt version 4.0 or higher is required, you have another version!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the base path to the right Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <QWidget>
#if !defined(QT_VERSION) || (QT_VERSION < 0x040000)
# error Wrong Qt version
#endif
])],,AC_MSG_ERROR(${QT4_WRONG_VESION}))

# publish:
AC_SUBST(QT4CORE_CPPFLAGS)
AC_SUBST(QT4CORE_LDFLAGS)
AC_SUBST(QT4CORE_LIBS)
AC_SUBST(QT4_CPPFLAGS)
AC_SUBST(QT4_LDFLAGS)
AC_SUBST(QT4_LIBS)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}


# moc:
FORCED_MOC=
AC_ARG_WITH([moc],
	[AS_HELP_STRING([--with-moc=CMD],[override moc command])],
	[QT4_MOC_ERROR="No path given for option --with-moc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		FORCED_MOC=${withval}
	else
		AC_MSG_ERROR(${QT4_MOC_ERROR})
	fi],
	[])

QT4_MOC_VERSION_ERROR="$MOC is not for Qt 4 (wrong version)!
   Please specify the full path to Moc:
   e.g. './configure --with-moc=/usr/lib/qt4/bin/moc'
   or provide the base path to the Qt4 installation:
   e.g. './configure --with-qt4=/usr/lib/qt4'."
if test "x${FORCED_MOC}" != x ; then
    # Moc command passed to configure
    MOC=${FORCED_MOC}
    if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
	    AC_MSG_ERROR(${QT4_MOC_VERSION_ERROR})
    fi
else
    if test "x${EXTRA_MOC_LOCATION}" != x ; then
        # Moc from Qt4 directory
        MOC="${EXTRA_MOC_LOCATION}/moc"
        if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
            # Moc-qt4 from path
            MOC=moc-qt4
            if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
                # Moc from path
                MOC=moc
                if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
	                AC_MSG_ERROR(${QT4_MOC_VERSION_ERROR})
                fi
            fi
        fi
    else
        # Moc-qt4 from path
        MOC=moc-qt4
        if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
            # Moc from path
            MOC=moc
            if ! "${MOC}" -v 2>&1 | grep 'Qt 4' &>/dev/null ; then
                AC_MSG_ERROR(${QT4_MOC_VERSION_ERROR})
            fi
        fi
    fi
fi

AC_SUBST(MOC)

])

