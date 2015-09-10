# code modified from http://www.doeben-henisch.de/fh/II-PRT-SS05/VL/VL6/rtai-guide-local.html
# DIAPM RTAI - Beginner's Guide
# Makefile by Jan Benda

ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I /usr/include -I. -I/usr/realtime/include -ffast-math -mhard-float -mmmx -msse -msse2 -mssse3 -msse4.1 -msse4.2
EXTRA_CFLAGS += -mpreferred-stack-boundary=4
EXTRA_CFLAGS += -Wall

KBUILD_EXTRA_SYMBOLS := /usr/realtime/modules/Module.symvers

obj-m := rt_process.o

else

KERNELDIR=/usr/src/linux

all: module scope

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

scope: scope.c
	gcc -o $@ $<

clean:
	rm scope
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean


distclean: clean
	rm -f modules.order

help:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) help

dvi:
html:
info:
distdir:
mostlyclean: clean
check:
install:
uninstall:
installcheck:

endif
