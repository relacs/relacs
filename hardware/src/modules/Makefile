ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS += -I/usr/src/kernels/rtai-3.2/include/ -I/usr/src/comedi/include/ -I/usr/include/ -ffast-math -mhard-float

EXTRA_CFLAGS += -Wall

obj-m := dynclamp.o
dynclamp-objs := rtmodule.o model.o blocklist.o

else

LINUXDIR := /lib/modules/`uname -r`/build
PWD     := $(shell pwd)
CC := gcc32

all: logdata module

module:
	$(MAKE) -C $(LINUXDIR) CC=$(CC) SUBDIRS=$(PWD) modules

#logdata: logdata.c diffstep.o
#	$(CC) -o logdata $^ -lm

#diffstep.o: diffstep.c diffstep.h
#	$(CC) -c diffstep.c


clean:
	-rm -f *.ko *.o *.mod.c logdata

endif

