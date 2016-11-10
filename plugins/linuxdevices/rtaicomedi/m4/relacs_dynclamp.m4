# RELACS_DYNCLAMP() 
# Makes features of dynclamp module configurable and summarize the enabled features in RELACS_DYNCLAMP_SUMMARY

AC_DEFUN([RELACS_DYNCLAMP], [

RELACS_DYNCLAMP_SUMMARY=""

#    AC_ARG_ENABLE([dynclamp_computation],
#        [AS_HELP_STRING([--enable-dynclamp-computation],[Compute a model in each cycle of the dynamic clamp loop])],
#        [AS_IF([test "x$enableval" != "xno"],[AC_DEFINE( ENABLE_COMPUTATION ); RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY comp"])], [])
    AC_DEFINE( ENABLE_COMPUTATION )
    RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY comp"

#    AC_ARG_ENABLE([dynclamp_mathh],
#        [AS_HELP_STRING([--enable-dynclamp-mathh],[include rtai_math.h])],
#        [AS_IF([test "x$enableval" != "xno"],[AC_DEFINE( ENABLE_MATHH ); RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY mathh"])], [])
    AC_DEFINE( ENABLE_MATHH )
    RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY mathh"

    AC_ARG_ENABLE([dynclamp_lookuptables],
        [AS_HELP_STRING([--enable-dynclamp-lookuptables],[Use lookup tables instead of math library for the dynamic clamp model.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY lookup"; AC_DEFINE( ENABLE_LOOKUPTABLES )])], [])

    AC_ARG_ENABLE([dynclamp_trigger],
        [AS_HELP_STRING([--enable-dynclamp-trigger],[Allow analog output to be triggered on some event.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY trig"; AC_DEFINE( ENABLE_TRIGGER )])], [])

    AC_ARG_ENABLE([dynclamp_ttlpulse],
        [AS_HELP_STRING([--enable-dynclamp-ttlpulse],[Enable generation of TTL pulses at certain time points within the dynamic clamp loop.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY ttl"; AC_DEFINE( ENABLE_TTLPULSE )])], [])

    AC_ARG_ENABLE([dynclamp_syncsec],
        [AS_HELP_STRING([--enable-dynclamp-syncsec],[Enable the generation of the TTL pulses needed for synchronizing the SEC discontinuous mode with the dynamic clamp loop.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY sync"; AC_DEFINE( ENABLE_SYNCSEC )])], [])

    AC_ARG_ENABLE([dynclamp_aitime],
        [AS_HELP_STRING([--enable-dynclamp-aitime],[Make time spent for reading in data available as input trace AI-time.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY ai-t"; AC_DEFINE( ENABLE_AITIME )])], [])

    AC_ARG_ENABLE([dynclamp_aiacquisitiontime],
        [AS_HELP_STRING([--enable-dynclamp-aiacquisitiontime],[Make time spent for a single analog input acquisition available as input trace AI-acquisition-time.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY aiacq-t; AC_DEFINE( ENABLE_AIACQUISITIONTIME )"])], [])

    AC_ARG_ENABLE([dynclamp_aotime],
        [AS_HELP_STRING([--enable-dynclamp-aotime],[Make time spent for analog output available as input trace AO-time.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY ao-t"; AC_DEFINE( ENABLE_AOTIME )])], [])

    AC_ARG_ENABLE([dynclamp_modeltime],
        [AS_HELP_STRING([--enable-dynclamp-modeltime],[Make time spent for computing the model available as input trace Model-time.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY model-t"; AC_DEFINE( ENABLE_MODELTIME )])], [])

    AC_ARG_ENABLE([dynclamp_],
        [AS_HELP_STRING([--enable-dynclamp-waittime],[Make time spent for waiting for the next period available as input trace Wait-time.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY wait-t"; AC_DEFINE( ENABLE_WAITTIME )])], [])

    AC_ARG_ENABLE([dynclamp_oneshot],
        [AS_HELP_STRING([--enable-dynclamp-oneshot],[Use one-shot instead of periodic mode for timing of dynamic clamp loop.])],
        [AS_IF([test "x$enableval" != "xno"],[RELACS_DYNCLAMP_SUMMARY="$RELACS_DYNCLAMP_SUMMARY oneshot"; AC_DEFINE( ONESHOT_MODE )])], [])


AS_IF([test -n "$RELACS_DYNCLAMP_SUMMARY"],[RELACS_DYNCLAMP_SUMMARY="( with$RELACS_DYNCLAMP_SUMMARY )"])

])
