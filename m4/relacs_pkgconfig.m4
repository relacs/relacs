# RELACS_CHECK_PKG_CONFIG() 
# - Checks for the existance of the pkg-config program
# - Issues a warning message, if pkg-config is not found
# - On success, sets the PKG_CONFIG variable to the executable


AC_DEFUN([RELACS_CHECK_PKG_CONFIG], [

PKG_CONFIG_MISSING="pkg-config not found!
   You should install pkg-conig on your system.
   On a Debian-based system enter 'sudo apt-get install pkg-config'."

AC_ARG_VAR( [PKG_CONFIG], [the binary of the existing pkg-config program, otherwise empty] )

AC_PATH_PROG( [PKG_CONFIG], [pkg-config], [no] )

if test $PKG_CONFIG = "no" ; then
    AC_MSG_WARN( [ $PKG_CONFIG_MISSING ] )
fi

])

