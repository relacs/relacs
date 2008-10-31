#
#  NI E-Series Driver, Jan Benda
#  updated to 2.6. July 2005
#

MAJOR = 251
NAME = nidaq
VERSION = 1.3
DATE = $(shell date '+%D')
DIOOUT = 0xEF   # DIO pins that are configure for writing and set to zero at startup
SUBDEVS = niai niao nidio nipfi
BACKUPFILES = Makefile TODO mkdevices *.h *.c \
              examples/*.h examples/*.c examples/Makefile
#CC=gcc32

# DEFS    += -DNIDAQ_AIDEBUG
# DEFS    += -DNIDAQ_AODEBUG
# DEFS    += -DNIDAQ_DIODEBUG
# DEFS    += -DNIDAQ_COREDEBUG
# DEFS    += -DNIDAQ_MISCDEBUG
# DEFS    += -DNIDAQ_EPROMDEBUG
# DEFS    += -DNIDAQ_NO_USE_COUNT


ifneq ($(KERNELRELEASE),)

EXTRA_CFLAGS  += $(DEFS) -DNIDAQ_MAJOR=$(MAJOR) -DNIDAQ_NAME=\"$(NAME)\"
EXTRA_CFLAGS  += -DNIDAQ_VERSION=\"$(VERSION)\" -DNIDAQ_DATE=\"$(DATE)\" -DDIOOUT=$(DIOOUT)
EXTRA_LDFLAGS +=
EXTRA_ARFLAGS +=

obj-m := nidaq.o
nidaq-objs := core.o misc.o eprom.o ai.o ao.o dio.o pfi.o


else


KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
ALIAS := "alias char-major-$(MAJOR) nidaq"


.PHONY: module dev install backup clean


module:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


dev:
	./mkdevices $(MAJOR) $(SUBDEVS)


install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules_install  # copy nidaq.ko to /lib/modules/`uname -r`/extra
	grep -q $(ALIAS) /etc/modprobe.conf || { echo $(ALIAS) >>  /etc/modprobe.conf; }
	/sbin/depmod


backup:
	tar czvf nidaq.tgz $(BACKUPFILES)


clean:
	rm -f *.o *.ko *~ *.tgz


endif
