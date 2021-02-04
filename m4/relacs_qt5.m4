# RELACS_LIB_QT5() 
# - Provides --with-qt5(-(inc|lib))? options and performs header and link checks
# - Fills QTCORE_(LD|CPP)FLAGS and QTCORE_LIBS with values for the QtCore library and marks them for substitution
# - Fills QT_(LD|CPP)FLAGS and QT_LIBS with values for the QtCore, QtGui, and QtNetwork library and marks them for substitution
# - Fills MOC and marks it for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched

AC_DEFUN([RELACS_LIB_QT5], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# QT5 flags:
QTCORE_CPPFLAGS=
QTCORE_LDFLAGS=
QTCORE_LIBS=
QT_CPPFLAGS=
QT_LDFLAGS=
QT_LIBS=

# get flags:
if test "x${PKG_CONFIG}" != "x" && ${PKG_CONFIG} --exists Qt5Gui ; then
    QTCORE_CPPFLAGS="`${PKG_CONFIG} --cflags Qt5Core`"
    QTCORE_LDFLAGS="`${PKG_CONFIG} --libs-only-L Qt5Core`"
    QTCORE_LIBS="`${PKG_CONFIG} --libs-only-l Qt5Core`"
    QT_CPPFLAGS="`${PKG_CONFIG} --cflags Qt5Core` `${PKG_CONFIG} --cflags Qt5Gui` `${PKG_CONFIG} --cflags Qt5Network` `${PKG_CONFIG} --cflags Qt5Widgets` "
    QT_LDFLAGS="`${PKG_CONFIG} --libs-only-L Qt5Core` `${PKG_CONFIG} --libs-only-L Qt5Gui` `${PKG_CONFIG} --libs-only-L Qt5Network` `${PKG_CONFIG} --libs-only-L Qt5Widgets`"
    QT_LIBS="`${PKG_CONFIG} --libs-only-l Qt5Core` `${PKG_CONFIG} --libs-only-l Qt5Gui` `${PKG_CONFIG} --libs-only-l Qt5Network` `${PKG_CONFIG} --libs-only-l Qt5Widgets`"
fi

# default flags:
if test "x${QTCORE_CPPFLAGS}" = x ; then
    QTCORE_CPPFLAGS="-DQT_SHARED -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore"
fi
if test "x${QT_CPPFLAGS}" = x ; then
    QT_CPPFLAGS="-DQT_SHARED -I/usr/include/x86_64-linux-gnu/qt5 -I/usr/include/x86_64-linux-gnu/qt5/QtCore -I/usr/include/x86_64-linux-gnu/qt5/QtGui -I/usr/include/x86_64-linux-gnu/qt5/QtNetwork"
fi
if test "x${QTCORE_LIBS}" = x ; then
    QTCORE_LIBS="-lQtCore"
fi
if test "x${QT_LIBS}" = x ; then
    QT_LIBS="-lQtGui -lQtNetwork -lQtCore"
fi

# read arguments:
WITH_QT="yes"
EXTRA_MOC_LOCATION=
AC_ARG_WITH([qt5],
	[AS_HELP_STRING([--with-qt5=DIR],
	           	[override Qt5 path ("/lib" and "/include" is appended)])],
	[QT5_ERROR="no path given for option --with-qt5"
	if test ${withval} = no ; then
	   WITH_QT="no"
	elif test ${withval} != yes -a "x${withval}" != x ; then
	   	QTCORE_CPPFLAGS="-DQT_SHARED -I${withval}/include/x86_64-linux-gnu/qt5 -I${withval}/include/x86_64-linux-gnu/qt5/QtCore"
    		QT_CPPFLAGS="-DQT_SHARED -I${withval}/include/x86_64-linux-gnu/qt5 -I${withval}/include/x86_64-linux-gnu/qt5/QtCore -I${withval}/include/x86_64-linux-gnu/qt5/QtGui -I${withval}/include/x86_64-linux-gnu/qt5/QtNetwork"
		QTCORE_LDFLAGS="-L${withval}/lib"
		QT_LDFLAGS="-L${withval}/lib"
		EXTRA_MOC_LOCATION="${withval}/bin"
	else
		AC_MSG_ERROR(${QT5_ERROR})
	fi],
	[])

AC_ARG_WITH([qt5-inc],
	[AS_HELP_STRING([--with-qt5-inc=DIR],[override Qt5 include path])],
	[QT5_INC_ERROR="no path given for option --with-qt5-inc"
	if test ${withval} != yes -a "x${withval}" != x ; then
	   	QTCORE_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore"
    		QT_CPPFLAGS="-DQT_SHARED -I${withval} -I${withval}/QtCore -I${withval}/QtGui -I${withval}/QtCore -I${withval}/QtNetwork"	
	else
		AC_MSG_ERROR(${QT5_INC_ERROR})
	fi],
	[])

AC_ARG_WITH([qt5-lib],
	[AS_HELP_STRING([--with-qt5-lib=DIR],[override Qt5 library path])],
	[QT5_LIB_ERROR="no path given for option --with-qt5-lib"
	if test ${withval} != yes -a "x${withval}" != x ; then
		QTCORE_LDFLAGS="-L${withval}
		QT_LDFLAGS="-L${withval}
	else
		AC_MSG_ERROR(${QT5_LIB_ERROR})
	fi],
	[])

if test $WITH_QT = "yes"; then

# update flags:
CPPFLAGS="${QT_CPPFLAGS} ${CPPFLAGS}"
LDFLAGS="${QT_LDFLAGS} ${LDFLAGS}"

echo "###########################################################################"
echo "HEADER TESTS"
echo $LDFLAGS
echo $CPPFLAGS
echo "###########################################################################"

QT5_INC_MISSING="cannot find the header files for Qt5!
You either need to
- install a Qt5 development package (e.g. libqt5-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 include directory:
  e.g. './configure --with-qt4-inc=/usr/lib/qt4/include'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
CXXFLAGS = "-fPIC" ${CXXFLAGS}
AC_CHECK_HEADERS(QWidget QThread,, [
  echo "PING"
  CXXFLAGS="-fPIC ${CXXFLAGS}"
  echo $CXXFLAGS
  AC_CHECK_HEADERS(QWidget QThread,, AC_MSG_ERROR(${QT5_INC_MISSING}))
  ])

QT5CORE_LIB_MISSING="cannot find the Qt5 core libraries!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 libraries:
  e.g. './configure --with-qt4-lib=/usr/lib/qt4/lib'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_LIB(Qt5Core, main,, AC_MSG_ERROR(${QT5CORE_LIB_MISSING}), ${QTCORE_LDFLAGS})

QT5_LIB_MISSING="cannot find the Qt5 gui libraries!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 libraries:
  e.g. './configure --with-qt4-lib=/usr/lib/qt4/lib'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_LIB(Qt5Gui, main,, AC_MSG_ERROR(${QT5_LIB_MISSING}), ${QT_LDFLAGS})

QT5NETWORK_LIB_MISSING="cannot find the Qt5 network libraries!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the path to the Qt4 libraries:
  e.g. './configure --with-qt4-lib=/usr/lib/qt4/lib'
- or provide the base path to the Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_CHECK_LIB(Qt5Network, main,, AC_MSG_ERROR(${QT5NETWORK_LIB_MISSING}), ${QT_LDFLAGS})

QT5_WRONG_VESION="Qt version 5.0 or higher is required, you have another version!
You either need to
- install a Qt4 development package (e.g. libqt4-dev,
  on a Debian-based system enter 'sudo apt-get install libqt4-dev'),
- or provide the base path to the right Qt4 installation:
  e.g. './configure --with-qt4=/usr/lib/qt4'."
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <QWidget>
#if !defined(QT_VERSION) || (QT_VERSION < 0x050000)
# error Wrong Qt version
#endif
])],,AC_MSG_ERROR(${QT5_WRONG_VESION}))

fi
# WITH_QT

# publish:
AC_SUBST(QTCORE_CPPFLAGS)
AC_SUBST(QTCORE_LDFLAGS)
AC_SUBST(QTCORE_LIBS)
AC_SUBST(QT_CPPFLAGS)
AC_SUBST(QT_LDFLAGS)
AC_SUBST(QT_LIBS)

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
	[QT5_MOC_ERROR="No path given for option --with-moc"
	if test ${withval} != yes -a "x${withval}" != x ; then
		FORCED_MOC=${withval}
	else
		AC_MSG_ERROR(${QT5_MOC_ERROR})
	fi],
	[])

QT5_MOC_VERSION_ERROR="$MOC is not for Qt 5 (wrong version)!
   Please specify the full path to Moc:
   e.g. './configure --with-moc=/usr/lib/qt4/bin/moc'
   or provide the base path to the Qt4 installation:
   e.g. './configure --with-qt4=/usr/lib/qt4'."
if test "x${FORCED_MOC}" != x ; then
    # Moc command passed to configure
    MOC=${FORCED_MOC}
    if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
	    AC_MSG_ERROR(${QT5_MOC_VERSION_ERROR})
    fi
else
    if test "x${EXTRA_MOC_LOCATION}" != x ; then
        # Moc from Qt5 directory
        MOC="${EXTRA_MOC_LOCATION}/moc"
        echo "#################### MOC TEST####################"
        echo $MOC
        if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
            # Moc-qt4 from path
            MOC=moc-qt4
            if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
                # Moc from path
                MOC=moc
                if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
	                AC_MSG_ERROR(${QT5_MOC_VERSION_ERROR})
                fi
            fi
        fi
    else
        # Moc-qt5 from path
        MOC=moc
        echo "#################### MOC TEST####################"
        echo $MOC
        if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
            # Moc from path
            MOC=moc
            if ! "${MOC}" -v 2>&1 | fgrep ' 5' &>/dev/null ; then
                AC_MSG_ERROR(${QT5_MOC_VERSION_ERROR})
            fi
        fi
    fi
fi

fi

AC_SUBST(MOC)

])

