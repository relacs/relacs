/*
  nidaq.h
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _RELACS_HARDWARE_NIDAQ_H_
#define _RELACS_HARDWARE_NIDAQ_H_ 1

#include <asm/ioctl.h>


/* list of board IDs: */

#define VENDORID 0x1093

#define NI_PCI  0x40000000 /* PCI board */
#define NI_ESER 0x20000000 /* E series MIO board */
#define NI_8255 0x10000000 /* 8255 based DIO board */

#define NI_PCI_MIO_16E_1        (0x6070 | NI_ESER | NI_PCI)
#define NI_PXI_6070E            (0x6070 | NI_ESER | NI_PCI)
#define NI_PCI_6071E            (0x6071 | NI_ESER | NI_PCI)
#define NI_PXI_6071E            (0x6071 | NI_ESER | NI_PCI)
#define NI_PCI_MIO_16E_4        (0x6040 | NI_ESER | NI_PCI)
#define NI_PXI_6040E            (0x6040 | NI_ESER | NI_PCI)
#define NI_PCI_MIO_16XE_10      (0x6030 | NI_ESER | NI_PCI)
#define NI_PXI_6030E            (0x6030 | NI_ESER | NI_PCI)
#define NI_PCI_6031E            (0x6031 | NI_ESER | NI_PCI)
#define NI_PXI_6031E            (0x6031 | NI_ESER | NI_PCI)
#define NI_PCI_6032E            (0x6032 | NI_ESER | NI_PCI)
#define NI_PCI_6033E            (0x6033 | NI_ESER | NI_PCI)
#define NI_PCI_6035E            (0x6035 | NI_ESER | NI_PCI)
#define NI_PCI_6023E            (0x6023 | NI_ESER | NI_PCI)
#define NI_PCI_6024E            (0x6024 | NI_ESER | NI_PCI)
#define NI_PCI_6025E            (0x6025 | NI_ESER | NI_8255 | NI_PCI)
#define NI_PXI_6023E            (0x6023 | NI_ESER | NI_PCI)
#define NI_PXI_6024E            (0x6024 | NI_ESER | NI_PCI)
#define NI_PXI_6025E            (0x6025 | NI_ESER | NI_8255 | NI_PCI)
#define NI_PCI_MIO_16XE_50      (0x6011 | NI_ESER | NI_PCI)
#define NI_PXI_6011E            (0x6011 | NI_ESER | NI_PCI)
#define NI_PCI_6052E            (0x6052 | NI_ESER | NI_PCI)
#define NI_PCI_6711             (0x6711 | NI_ESER | NI_PCI)


/* number of boards in boards.h: */
#define KNOWN_BOARDS 22

/* maximum number of voltage ranges: */
#define MAXRANGES 10

/* hardware infos about a board: */

typedef struct {
    /* name of the daq-board: */
  char name[40];
    /* pci device id: */
  unsigned short deviceid;
    /* defines listed above: it includes the board class & bus (ISA or PCI): */
  unsigned int type;
    /* number of ai channels: */
  int aich;
    /* maximum number of ai channels the configuration buffer can hold: */
  int aimaxch;
    /* resolution of ai channels in bits: */
  int aibits;
    /* max ai sampling frequency: */
  int aimaxspl;
    /* analog input fifo size (number of data elements): */
  int aififo;
    /* analog input maximum voltage ranges in millivolt for unipolar mode: */
  long aiuniranges[MAXRANGES];
    /* analog input maximum voltage ranges in millivolt for bipolar mode: */
  long aibiranges[MAXRANGES];
    /* analog input gain indices corresponding to the ranges: */
  int aiindices[MAXRANGES];
    /* number of ao channels: */
  int aoch;
    /* resolution of ao channels in bits: */
  int aobits;
    /* max ao sampling frequency: */
  int aomaxspl;
    /* analog output fifo size (number of data elements): */
  int aofifo;
    /* number of digital input/output lines: */
  int dios;
} nidaq_info;


/* general ioctl: */

#define NIDAQINFO _IO( NIDAQ_MAJOR, 1 )
#define NIDAQTIMEBASE _IO( NIDAQ_MAJOR, 2 )
#define NIDAQLOWVALUE _IOW( NIDAQ_MAJOR, 3, int )
#define NIDAQHIGHVALUE _IOW( NIDAQ_MAJOR, 4, int )
#define NIDAQTRIGGERMODE _IOW( NIDAQ_MAJOR, 5, int )
#define NIDAQTRIGGERSOURCE _IOW( NIDAQ_MAJOR, 6, int )


/* pfi ioctl: */

#define NIDAQPFIOUT _IOW( NIDAQ_MAJOR, 7, int )
#define NIDAQPFIIN _IOW( NIDAQ_MAJOR, 8, int )


/* analog input ioctl: */

#define NIDAQAIRESETALL _IO( NIDAQ_MAJOR, 51 )
#define NIDAQAIRESET _IO( NIDAQ_MAJOR, 52 )
#define NIDAQAIRESETBUFFER _IO( NIDAQ_MAJOR, 53 )

#define NIDAQAICLEARCONFIG _IO( NIDAQ_MAJOR, 54 )
#define NIDAQAIADDCHANNEL _IOW( NIDAQ_MAJOR, 55, unsigned long )

#define NIDAQAISCANSTART _IOW( NIDAQ_MAJOR, 56, unsigned int )
#define NIDAQAISCANDELAY _IOW( NIDAQ_MAJOR, 57, unsigned long )
#define NIDAQAISCANINTERVAL _IOW( NIDAQ_MAJOR, 58, unsigned int )
#define NIDAQAISCANRATE _IOW( NIDAQ_MAJOR, 59, unsigned long )
#define NIDAQAISAMPLESTART _IOW( NIDAQ_MAJOR, 60, unsigned short )
#define NIDAQAISAMPLEDELAY _IOW( NIDAQ_MAJOR, 61, unsigned long )
#define NIDAQAISAMPLEINTERVAL _IOW( NIDAQ_MAJOR, 62, unsigned short )
#define NIDAQAISAMPLERATE _IOW( NIDAQ_MAJOR, 63, unsigned long )

#define NIDAQAISTART1SOURCE _IOW( NIDAQ_MAJOR, 64, int )
#define NIDAQAISTART1POLARITY _IOW( NIDAQ_MAJOR, 65, int )
#define NIDAQAIANALOGTRIGGER _IO( NIDAQ_MAJOR, 66 )

#define NIDAQAICONTINUOUS _IOW( NIDAQ_MAJOR, 67, int )
#define NIDAQAIENDONSCAN _IO( NIDAQ_MAJOR, 68 )

#define NIDAQAIERROR _IOR( NIDAQ_MAJOR, 69, unsigned long )
#define NIDAQAIDATA _IOR( NIDAQ_MAJOR, 70, unsigned long )
#define NIDAQAIRUNNING _IO( NIDAQ_MAJOR, 71 )


/* analog output ioctl: */

#define NIDAQAORESETALL _IO( NIDAQ_MAJOR, 100 )
#define NIDAQAORESET _IO( NIDAQ_MAJOR, 101 )
#define NIDAQAORESETBUFFER _IO( NIDAQ_MAJOR, 102 )

#define NIDAQAOCLEARCONFIG _IO( NIDAQ_MAJOR, 103 )
#define NIDAQAOADDCHANNEL _IOW( NIDAQ_MAJOR, 104, unsigned short )

#define NIDAQAOSTART _IOW( NIDAQ_MAJOR, 105, unsigned int )
#define NIDAQAODELAY _IOW( NIDAQ_MAJOR, 106, unsigned long )
#define NIDAQAOINTERVAL _IOW( NIDAQ_MAJOR, 107, unsigned int )
#define NIDAQAORATE _IOW( NIDAQ_MAJOR, 108, unsigned long )
#define NIDAQAOBUFFERS _IOW( NIDAQ_MAJOR, 109, unsigned short )
#define NIDAQAOSTAGING _IOW( NIDAQ_MAJOR, 110, int )

#define NIDAQAOMUTEUPDATES _IOW( NIDAQ_MAJOR, 111, unsigned short )
#define NIDAQAOMUTEBUFFERS _IOW( NIDAQ_MAJOR, 112, unsigned short )

#define NIDAQAOSTART1SOURCE _IOW( NIDAQ_MAJOR, 113, int )
#define NIDAQAOSTART1POLARITY _IOW( NIDAQ_MAJOR, 114, int )
#define NIDAQAOANALOGTRIGGER _IO( NIDAQ_MAJOR, 115 )

#define NIDAQAOTRIGGERONCE _IO( NIDAQ_MAJOR, 116 )
#define NIDAQAOENDONBCTC _IO( NIDAQ_MAJOR, 117 )

#define NIDAQAOBUFFERSTART _IOR( NIDAQ_MAJOR, 118, unsigned long )
#define NIDAQAOERROR _IOR( NIDAQ_MAJOR, 119, unsigned long )
#define NIDAQAORUNNING _IO( NIDAQ_MAJOR, 120 )
#define NIDAQAOISSTAGING _IO( NIDAQ_MAJOR, 121 )


/* digital input/output ioctl: */

#define NIDAQDIOCONFIGURE _IOW( NIDAQ_MAJOR, 151, int ) /* 0: input, 1: output */
#define NIDAQDIOMASK _IOW( NIDAQ_MAJOR, 152, int )  /* which lines to use for digital parallel output */
#define NIDAQDIOPAROUT _IOW( NIDAQ_MAJOR, 153, char ) /* parallel output */
#define NIDAQDIOPARIN _IOR( NIDAQ_MAJOR, 154, char )  /* parallel input */
#define NIDAQDIOCLOCK _IOWR( NIDAQ_MAJOR, 155, int )  /* set the EXTSTROBE/SDCLK pin */
#define NIDAQDIOTIMEDIV2 _IOWR( NIDAQ_MAJOR, 156, int )  /* divide timebase by two for serioal dio. */
#define NIDAQDIOTIMEBASE _IOWR( NIDAQ_MAJOR, 157, int )  /* use slow timebase for serial dio (default). */

#endif /* ! _RELACS_HARDWARE_NIDAQ_H_ */
