# AC_RELACS_PLUGIN_SET( pluginsetname, pluginsetdir, dependencies, externaldependency ) 
# Takes care of the plugin set with name $pluginsetname in 
# directory $pluginsetdir relative to plugins/.
# The plugin set depends on the plugin sets whose names are listed in $dependencies.
# The plugin set might depend on further external dependencies as 
# determined by the shell test $externaldependencies.
# If $pluginsetname is anabled and all dependencies are fullfilled, then
# - adds plugins/$pluginset to DOXYGEN_EXTERNAL
# - adds $pluginset to SUBDIRS in plugins/Makefile.am
# In any case adds $pluginset DIST_SUBDIRS in plugins/Makefile.am

AC_DEFUN([AC_RELACS_PLUGIN_SET], [

# check whether the plugin set should be compiled at all:
RELACS_COMPILE="yes"
AC_ARG_ENABLE(plugins-$1,
   [AS_HELP_STRING([--disable-plugins-$1],[don't compile $1 plugins])],
   [AS_IF([test "x$enableval" = xyes],[RELACS_COMPILE="yes"],[RELACS_COMPILE="no"])],
   [])

# is the external dependecy fullfilled?
RELACS_EXTERNAL_DEPENDENCY="$4"
if test "x$RELACS_EXTERNAL_DEPENDENCY" = x; then
    RELACS_EXTERNAL_DEPENDENCY=true
fi
if ! $RELACS_EXTERNAL_DEPENDENCY; then
    RELACS_COMPILE="no"
fi

# check for missing dependencies on other plugin sets:
RELACS_MISSING_PLUGINS=""
if test "x$RELACS_COMPILE" = "xyes"; then
    for RELACS_REQUIRED_PLUGIN in $3; do
	RELACS_PLUGIN_FOUND=no
        for RELACS_ENABLED_PLUGIN in $RELACS_PLUGINS; do
	    if test "x$RELACS_REQUIRED_PLUGIN" = "x$RELACS_ENABLED_PLUGIN"; then
	        RELACS_PLUGIN_FOUND=yes
		break
            fi
  	done
	if test "x$RELACS_PLUGIN_FOUND" = "xno"; then
            RELACS_COMPILE="no"
            RELACS_MISSING_PLUGINS="$RELACS_MISSING_PLUGINS $RELACS_REQUIRED_PLUGIN"
	fi
    done
fi
if test "x$RELACS_MISSING_PLUGINS" != "x"; then
    AC_MSG_ERROR([plugin set $2: dependencies on plugin sets$RELACS_MISSING_PLUGINS not fullfilled])
fi

# inform the user:
AC_MSG_NOTICE([enable plugin set $1: $RELACS_COMPILE ])

# add the plugin set to various variables:
if test "x$RELACS_COMPILE" = "xyes"; then
    DOXYGEN_EXTERNAL="${DOXYGEN_EXTERNAL} plugins/$2"
    RELACS_PLUGINS_SUBDIRS="${RELACS_PLUGINS_SUBDIRS} $2"
    RELACS_PLUGINS="${RELACS_PLUGINS} $1"
fi

# in any case, the plugin set should go into the distribution:
RELACS_PLUGINS_DIST_SUBDIRS="${RELACS_PLUGINS_DIST_SUBDIRS} $2"

])

