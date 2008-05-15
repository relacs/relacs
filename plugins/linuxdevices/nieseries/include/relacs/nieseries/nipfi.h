/*
  nieseries/nipfi.h
  Controls the "Programmable Function Inputs" (PFI)

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

#ifndef _RELACS_NIESERIES_NIPFI_H_
#define _RELACS_NIESERIES_NIPFI_H_ 1

#include <string>
#include <relacs/device.h>
using namespace std;
using namespace relacs;

namespace nieseries {


/*!
\class NIPFI
\brief Controls the "Programmable Function Inputs" (PFI)
of a National Instruments E Series board.
\author Jan Benda
\version 1.0

Programmed as an input the PFI pins allow to feed external signals
to the daq board.

Programmed as an output the PFI pins provide internal signals
that can be used, for example, to synchronise other daq boards
or as a trigger for an oscilloscope. The following internal
signals are available:
- pin 0 (PFI0): AI_START1, start trigger of ananlog input
- pin 1 (PFI1): AI_START2, second start trigger of ananlog input
- pin 2 (PFI2): CONV*, analog input CONVERT 
- pin 3 (PFI3): G_SRC1, G_SOURCE from general purpose counter 1 
- pin 4 (PFI4): G_GATE1, G_GATE from general purpose counter 1 
- pin 5 (PFI5): UPDATE*, analog output UPDATE
- pin 6 (PFI6): AO_START1, start trigger of analog output
- pin 7 (PFI7): AI_START, start trigger of analog input
- pin 8 (PFI8): G_SRC0, G_SOURCE from general purpose counter 0 
- pin 9 (PFI9): G_GATE0, G_GATE from general purpose counter 0 

 */

class NIPFI : public Device
{

public:

    /*! Construct and open the PFU subdevice. 
        For each bit (0-9) that is set in \a mode
        the corresponding PFI pin is programmed for output. */
  NIPFI( const string &device, long mode=0 );
    /*! Constructor. */
  NIPFI( void );
    /*! Destructor. Closes an open PFI subdevice. */
  ~NIPFI( void );

    /*! Open the PFI subdevice. For each bit (0-9) that is set in \a mode
        the corresponding PFI pin is programmed for output. */
  virtual int open( const string &device, long mode=0 );
    /*! Return \a true if the PFU subdevice is open. */
  virtual bool isOpen( void ) const;
    /*! Close the PFI subdevice. */
  virtual void close( void );

    /*! Programm PFI pin \a pin for input. */
  int pinIn( int pin );
    /*! Programm PFI pin \a pin for output. */
  int pinOut( int pin );


private:

    /*! Handle for the device. */
  int Handle;

};


}; /* namespace nieseries */

#endif /* ! _RELACS_NIESERIES_NIPFI_H_ */
