/*
  comedi/comedinipfi.h
  Controlling the PFI pins of a NI daq-board via comedi.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/device.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class ComediNIPFI
\author Jan Benda
\brief [Device] Controlling the PFI pins of a NI daq-board via comedi.
*/


class ComediNIPFI : public Device
{

public:

    /*! Create a new ComediNIPFI without opening a device. */
  ComediNIPFI( void );
    /*! Open comedi specified by its device file \a device. */
  ComediNIPFI( const string &device, long mode=0 );
    /*! Close the comedi driver. */
  virtual ~ComediNIPFI( void );

    /*! Open the PFI device specified by \a device.
 	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device, long mode=0 );
    /*! \return \c true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void );


private:

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;
    /*! The comedi subdevice number. */
  unsigned int SubDevice;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDINIPFI_H_ */
