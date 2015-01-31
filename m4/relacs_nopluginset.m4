# RELACS_NOPLUGINSET( pluginsetname, pluginsetdir, dependencies, externaldependency ) 
# Same as RELACS_PLUGINSET, but disable the pluginset by default.

AC_DEFUN([RELACS_NOPLUGINSET], [

# check whether the plugin set should be compiled at all:
RELACS_PLUGINSET_ENABLE="no"
AC_ARG_ENABLE(plugins-$1,
   [AS_HELP_STRING([--enable-plugins-$1],[do enable and compile $1 plugins])],
   [AS_IF([test "x$enableval" = xyes],[RELACS_PLUGINSET_ENABLE="yes"],[RELACS_PLUGINSET_ENABLE="no"])],
   [RELACS_PLUGINSET_ENABLE="no"])

RELACS_PLUGINSET_COMPILE="$RELACS_PLUGINSET_ENABLE"

# is the external dependency fullfilled?
RELACS_EXTERNAL_DEPENDENCY="$4"
if test "x$RELACS_EXTERNAL_DEPENDENCY" = x; then
    RELACS_EXTERNAL_DEPENDENCY=true
fi
if ! $RELACS_EXTERNAL_DEPENDENCY; then
    RELACS_PLUGINSET_COMPILE="no"
fi

# check for missing dependencies on other plugin sets:
if test "x$RELACS_PLUGINSET_ENABLE" = "xyes"; then
    if test "x$RELACS_PLUGINSET_COMPILE" = "xyes"; then
	RELACS_MISSING_PLUGINS=""
    	for RELACS_REQUIRED_PLUGIN in $3; do
	    RELACS_PLUGIN_FOUND=no
            for RELACS_PLUGIN in $RELACS_PLUGINS_COMPILE; do
	    	if test "x$RELACS_REQUIRED_PLUGIN" = "x$RELACS_PLUGIN"; then
	           RELACS_PLUGIN_FOUND=yes
		   break
            	fi
  	    done
	    if test "x$RELACS_PLUGIN_FOUND" = "xno"; then
               RELACS_PLUGINSET_ENABLE="no"
               RELACS_PLUGINSET_COMPILE="no"
               RELACS_MISSING_PLUGINS="$RELACS_MISSING_PLUGINS $RELACS_REQUIRED_PLUGIN"
	    fi
    	done
    	AS_IF( [test "x$RELACS_MISSING_PLUGINS" != "x"],
    	       [AC_MSG_ERROR([plugin set $2: dependencies on plugin sets$RELACS_MISSING_PLUGINS not fulfilled !])] )
    else
	RELACS_MISSING_PLUGINS=""
    	for RELACS_REQUIRED_PLUGIN in $3; do
	    RELACS_PLUGIN_FOUND=no
            for RELACS_PLUGIN in $RELACS_PLUGINS_COMPILE $RELACS_PLUGINS_ENABLE; do
	    	if test "x$RELACS_REQUIRED_PLUGIN" = "x$RELACS_PLUGIN"; then
	           RELACS_PLUGIN_FOUND=yes
		   break
            	fi
  	    done
	    if test "x$RELACS_PLUGIN_FOUND" = "xno"; then
               RELACS_PLUGINSET_ENABLE="no"
               RELACS_PLUGINSET_COMPILE="no"
               RELACS_MISSING_PLUGINS="$RELACS_MISSING_PLUGINS $RELACS_REQUIRED_PLUGIN"
	    fi
    	done
    	AS_IF( [test "x$RELACS_MISSING_PLUGINS" != "x"],
    	       [AC_MSG_ERROR([plugin set $2: dependencies on plugin sets$RELACS_MISSING_PLUGINS not fulfilled !])] )
    fi
fi

# inform the user:
PLUGIN_STATUS=""
AS_IF([test "x$RELACS_PLUGINSET_ENABLE" = "xyes" ],
      AS_IF([test "x$RELACS_PLUGINSET_COMPILE" = "xyes" ],
            [PLUGIN_STATUS="document and compile"],
	    [PLUGIN_STATUS="document"]),
      [PLUGIN_STATUS="disabled"])
AC_MSG_NOTICE([plugin set $1: $PLUGIN_STATUS ])

# add the plugin set to various variables and definen am conditionals:
if test "x$RELACS_PLUGINSET_ENABLE" = "xyes"; then
    DOXYGEN_EXTERNAL="${DOXYGEN_EXTERNAL} plugins/$2"
    RELACS_PLUGINS_SUBDIRS="${RELACS_PLUGINS_SUBDIRS} $2"
    AM_CONDITIONAL(RELACS_COND_$1,true)
    if test "x$RELACS_PLUGINSET_COMPILE" = "xyes"; then
       RELACS_PLUGINS_COMPILE="${RELACS_PLUGINS_COMPILE} $1"
       AM_CONDITIONAL(RELACS_COND_COMPILE_$1,true)
    else
	RELACS_PLUGINS_ENABLE="${RELACS_PLUGINS_ENABLE} $1"
	AM_CONDITIONAL(RELACS_COND_COMPILE_$1,false)
    fi
else
    AM_CONDITIONAL(RELACS_COND_$1,false)
    AM_CONDITIONAL(RELACS_COND_COMPILE_$1,false)
fi

# in any case, the plugin set should go into the distribution:
RELACS_PLUGINS_DIST_SUBDIRS="${RELACS_PLUGINS_DIST_SUBDIRS} $2"
# ... and the Makefiles should be configured:
AC_CONFIG_FILES([ plugins/$2/Makefile plugins/$2/src/Makefile ])

])

