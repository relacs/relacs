#define NIDAQBOARDSDEF
#include "core.h"
#include "boards.h"
#undef NIDAQBOARDSDEF

#ifdef NIDAQ_COREDEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif

#if defined(KERNEL260)

#define NIDAQ_NO_USE_COUNT
#include "core260.c"

#else

#error: Kernel Version not supported!

#endif
