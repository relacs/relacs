# RELACS_CHECK_CXX() 
# - Checks for the existance of the C++ compiler in the CXX variable
# - Aborts with an error message, if the C++ compiler does not exist
# - This should be done by AC_PROG_CXX,
#   but this seems not to be the case (with autoconf version 2.61).
#   As a consequence, subsequent library tests fail and lead to
#   confusing error messages.


AC_DEFUN([RELACS_CHECK_CXX], [

CXX_MISSING="no C++ compiler found!
   Please specify a valid C++ compiler in the CXX environment variable 
   (e.g. execute something like 'CXX=g++-4.1 ./configure' )
   or install a C++ compiler on your system.
   On a Debian-based system enter 'sudo apt-get install g++'."

AC_CHECK_PROG( [HAVE_CXX], [$CXX], [yes] )

if test "x${HAVE_CXX}" = "x" ; then
    AC_MSG_ERROR( [ $CXX_MISSING ] )
fi

])

