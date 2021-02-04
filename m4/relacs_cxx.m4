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

# C++ language standard support
#  (default to c++11 for now)
AC_ARG_WITH([cxx_std],
  [AS_HELP_STRING([--with-cxx-std],
    [set the language standard to use @<:@default=c++11@:>@])],
  [],
  [with_cxx_std=c++11])

CXXSTD=
AS_IF([test "x$with_cxx_std" != xno], [
  CXXFLAGS="${CXXFLAGS} -std=$with_cxx_std"
  CXXSTD="$with_cxx_std"
])

# hack to allow qt5 compiling ...
CXXFLAGS="${CXXFLAGS} -fPIC"

#check for constexpr keyword support
AC_TRY_COMPILE([
constexpr int the_answer() { return 42; }
               ], [],
               have_constexpr=yes,
	       have_constexpr=no)

AS_IF([test "x$have_constexpr" != xno], [
  AC_DEFINE([HAVE_CONSTEXPR], [1], [Define if the constexpr keyword is supported])
  AC_MSG_NOTICE([cxx has constexpr support])
])

])

