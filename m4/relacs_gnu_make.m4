# AC_RELACS_GNU_MAKE() 
# - Check whether make is GNU make

AC_DEFUN([AC_RELACS_GNU_MAKE], [
NON_GNU_MAKE="make is not GNU make. Please fix this."
if ! make --version | grep 'GNU' &>/dev/null ; then
	AC_MSG_ERROR(${NON_GNU_MAKE})
fi
])

