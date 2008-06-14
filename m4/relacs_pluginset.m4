# AC_RELACS_PLUGIN_SET( pluginset ) 
# - adds plugins/$pluginset to DOXYGEN_EXTERNAL

# next steps:
# 1) add plugin path to list of subdirs and distsubdirs in plugins/Makefile.am 
# 2) remove plugins/linuxdevices/Makefile.am and put all subdirs in plugins/Makefile.am
# 3) make the subdirs disable/enable
# 4) check for dependencies

AC_DEFUN([AC_RELACS_PLUGIN_SET], [

PLUGINSETDIR="plugins/$1"

DOXYGEN_EXTERNAL="${DOXYGEN_EXTERNAL} ${PLUGINSETDIR}"

])

