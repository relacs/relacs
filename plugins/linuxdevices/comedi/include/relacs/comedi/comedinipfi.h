/*
  comedi/comedinipfi.h
  Controlls the PFI pins of a NI daq-board via comedi.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#ifndef _COMEDI_COMEDINIPFI_H_
#define _COMEDI_COMEDINIPFI_H_

#include <comedilib.h>
#include <vector>
#include <relacs/comedi/comedirouting.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class ComediNIPFI
\author Jan Benda
\brief [Device] Controlls the PFI pins of a NI M-series daq-board via comedi.

The signal \a routing is routed to the channel \a channel on the 
PFI subdevice (subdevice no. 7) of a national instruments M-series  board.

Use for \a routing either the following integer values or
the names without the "NI_PFI_OUTPUT_" part (from comedi.h):
\code
enum ni_pfi_routing {
         NI_PFI_OUTPUT_PFI_DEFAULT = 0,
         NI_PFI_OUTPUT_AI_START1 = 1,
         NI_PFI_OUTPUT_AI_START2 = 2,
         NI_PFI_OUTPUT_AI_CONVERT = 3,
         NI_PFI_OUTPUT_G_SRC1 = 4,
         NI_PFI_OUTPUT_G_GATE1 = 5,
         NI_PFI_OUTPUT_AO_UPDATE_N = 6,
         NI_PFI_OUTPUT_AO_START1 = 7,
         NI_PFI_OUTPUT_AI_START_PULSE = 8,
         NI_PFI_OUTPUT_G_SRC0 = 9,
         NI_PFI_OUTPUT_G_GATE0 = 10,
         NI_PFI_OUTPUT_EXT_STROBE = 11,
         NI_PFI_OUTPUT_AI_EXT_MUX_CLK = 12,
         NI_PFI_OUTPUT_GOUT0 = 13,
         NI_PFI_OUTPUT_GOUT1 = 14,
         NI_PFI_OUTPUT_FREQ_OUT = 15,
         NI_PFI_OUTPUT_PFI_DO = 16,
         NI_PFI_OUTPUT_I_ATRIG = 17,
         NI_PFI_OUTPUT_RTSI0 = 18,
         NI_PFI_OUTPUT_PXI_STAR_TRIGGER_IN = 26,
         NI_PFI_OUTPUT_SCXI_TRIG1 = 27,
         NI_PFI_OUTPUT_DIO_CHANGE_DETECT_RTSI = 28,
         NI_PFI_OUTPUT_CDI_SAMPLE = 29,
         NI_PFI_OUTPUT_CDO_UPDATE = 30
};
\endcode

\par Options
- \c channel
- \c routing
*/


class ComediNIPFI : public ComediRouting
{

public:

    /*! Create a new ComediNIPFI without opening a device. */
  ComediNIPFI( void );
    /*! Open comedi specified by its device file \a device. */
  ComediNIPFI( const string &device, const Options &opts );
    /*! Close the comedi driver. */
  virtual ~ComediNIPFI( void );

    /*! Open the PFI device specified by \a device.
 	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device, const Options &opts );


protected:

  static const int PFISignalsMax = 31;
  static const string PFISignals[PFISignalsMax];

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDINIPFI_H_ */
