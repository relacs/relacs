#ifndef _RTMODULE_H_
#define _RTMODULE_H_

#include "moduledef.h"


// *** KERNEL LOGGING STYLE ***


#ifdef __KERNEL__
#  define ERROR_MSG(msg, args...) printk( KERN_ERR "rtmodule: " msg, ## args )
#else
#  define ERROR_MSG(msg, args...) fprintf( stderr, msg, ## args )
#endif

#ifdef __KERNEL__
#  define WARN_MSG(msg, args...) printk( KERN_WARNING "rtmodule: " msg, ## args )
#else
#  define WARN_MSG(msg, args...) fprintf( stderr, msg, ## args )
#endif

#ifdef RTMODULE_INFO
#  ifdef __KERNEL__
#    define INFO_MSG(msg, args...) printk( "rtmodule: " msg, ## args )
#  else
#    define INFO_MSG(msg, args...) fprintf( stderr, msg, ## args )
#  endif
#else
#  define INFO_MSG(msg, args...)
#endif

#ifdef RTMODULE_DEBUG
#  ifdef __KERNEL__
#    define DEBUG_MSG(msg, args...) printk( "rtmodule: " msg, ## args )
#  else
#    define DEBUG_MSG(msg, args...) fprintf( stderr, msg, ## args )
#  endif
#else
#  define DEBUG_MSG(msg, args...)
#endif


#endif
