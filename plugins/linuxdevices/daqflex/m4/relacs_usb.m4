# RELACS_LIB_USB() 
# - Provides --with(out)?-usb options and performs header and link checks
# - Fills (USB_(LD|CPP)FLAGS|LIBS) and marks them for substitution
# - Leaves ((LD|CPP)FLAGS|LIBS) untouched
# - Sets RELACS_USB with the result of the tests

AC_DEFUN([RELACS_LIB_USB], [

# save flags:
SAVE_CPPFLAGS=${CPPFLAGS}
SAVE_LDFLAGS=${LDFLAGS}
SAVE_LIBS=${LIBS}

# libusb flags:
USB_LDFLAGS=
USB_CPPFLAGS=
USB_LIBS=

RELACS_USB=no

# the following lines are a quick workaround for a properly installed libusb-1.0:
#USB_LIBS="-lusb-1.0"
#RELACS_USB=yes

# the following is the real script, but it does not work yet...
# read arguments:
AC_ARG_WITH([libusb],
  [AS_HELP_STRING([--with-libusb=DIR],[set libusb path ("/lib" and "/include" is appended)])
   AS_HELP_STRING([--without-libusb],[don\'t use libusb, i.e. prevent auto-detection])],
  [
	USB_ERROR="No path given for option --with-libusb"
	AS_IF([test "x$with_libusb" != xyes -a "$xwith_libusb" != xcheck -a "x$with_libusb" != xno -a "x$with_libusb" != x],[
		USB_CPPFLAGS="-I${with_libusb}/include"
		USB_LDFLAGS="-L${with_libusb}/lib"
		CPPFLAGS="${USB_CPPFLAGS} ${CPPFLAGS}"
		LDFLAGS="${USB_LDFLAGS} ${LDFLAGS}"
	        ],
              [test "x$with_libusb" = xyes],
		[AC_MSG_ERROR(${USB_ERROR})],
              [])
],
[
	# no usb argument given
	with_libusb=detect
])

# check usb:
USB_MISSING="libusb not found in path ${with_libusb}."
AS_IF([test "x$with_libusb" != xno],
  [SUCCESS=yes
   AC_CHECK_HEADERS([libusb-1.0/libusb.h],
     [if test "x$with_libusb" != xdetect; then
        RELACS_USB=$with_libusb
      else
        RELACS_USB=yes
      fi
     ], 
     [if test "x$with_libusb" != xdetect; then
        AC_MSG_ERROR(${USB_MISSING})
      fi
      SUCCESS=no
     ])
   AC_CHECK_LIB([usb-1.0], [main],
     [USB_LIBS="-lusb-1.0"
      if test "x$with_libusb" != xdetect; then
        RELACS_USB=$with_libusb
      else
        RELACS_USB=yes
      fi
     ], 
     [if test "x$with_libusb" != xdetect; then
        AC_MSG_ERROR(${USB_MISSING})
      fi
      SUCCESS=no
     ])
   AS_IF([test $SUCCESS = no],[RELACS_USB=no])
  ],
  [ RELACS_USB=no ] )

# publish:
AC_SUBST(USB_LDFLAGS)
AC_SUBST(USB_CPPFLAGS)
AC_SUBST(USB_LIBS)

# restore:
LDFLAGS=${SAVE_LDFLAGS}
CPPFLAGS=${SAVE_CPPFLAGS}
LIBS=${SAVE_LIBS}

])

