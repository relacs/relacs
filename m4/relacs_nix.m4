# RELACS_LIB_NIX()
# - Checks for the standard C math libray (-lm)
# - Fills MATH_LIBS and marks it for substitution
# - Leaves LIBS untouched

AC_DEFUN([RELACS_LIB_NIX], [

# the flags
NIX_CPPFLAGS=
NIX_LDFLAGS=
NIX_LIBS=
NIX_VERSION=

RELACS_NIX=no

# check for nix:
AC_ARG_WITH([nix],
	[AS_HELP_STRING([--with-nix], [enable NIX IO support @<:@default=check@:>@])],
	[],
	[with_nix=check])

#first check to see if we have a recent enough compiler standard version
# to compile with nix support
if test "x$with_nix" != "xno"; then

   have_ng_cxx=no
   for stdver in 0x 11 1y; do
     if test x"c++$stdver" == x"$CXXSTD" || test x"gnuc++$stdver" == x"$CXXSTD"; then
       have_ng_cxx=yes
       break;
     fi
   done

   if test "x$have_ng_cxx" != xyes; then
      if test "x$with_nix" != xcheck; then
        AC_MSG_FAILURE([NIX IO enabled but C++ standard not support])
      fi
      with_nix=no
   fi
fi

if test "x$with_nix" != "xno"; then

   echo -n "checking for nix..."
   if test "x${PKG_CONFIG}" != "x" && ${PKG_CONFIG} --exists nixio; then
      RELACS_NIX=yes
      NIX_CPPFLAGS="`${PKG_CONFIG} --cflags nixio`"
      NIX_LIBS="`${PKG_CONFIG} --libs-only-l nixio`"
      NIX_LDFLAGS="`${PKG_CONFIG} --libs-only-L nixio`"
      NIX_VERSION="`${PKG_CONFIG} --modversion nixio`"
      AC_DEFINE([HAVE_NIX], [1], [Define if you have libnixio])
      echo " yes ($NIX_VERSION)"
   else
      echo " no"
      if test "x$with_nix" != xcheck; then
        AC_MSG_FAILURE([NIX IO enabled but libnix not found])
      fi
   fi

   unset have_ng_cxx
fi

# publish:
AC_SUBST([NIX_CPPFLAGS])
AC_SUBST([NIX_LDFLAGS])
AC_SUBST([NIX_LIBS])

])
