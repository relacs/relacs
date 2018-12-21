/*
  comedi/comedirouting.h
  Route a signal to a channel of a subdevice.

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

#ifndef _COMEDI_COMEDIROUTING_H_
#define _COMEDI_COMEDIROUTING_H_

#include <comedilib.h>
#include <vector>
#include <relacs/device.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class ComediRouting
\author Jan Benda
\brief [Device] Route a signal to a channel of a subdevice.

The signal \a routing is routed to the channel \c channel on subdevice
\a subdevice and the channel is configured for output.

For routing signals to NI M-Series PFI lines, use the ComediNIPFI
plugin (this plugin knows that the PFI channels are at subdevice 7 and
provides meaningful names for the signals to be routed).

\par Options
- \c subdevice
- \c channel
- \c routing
*/


class ComediRouting : public Device
{

public:

    /*! Create a new ComediRouting without opening a device. */
  ComediRouting( void );
    /*! Create a new ComediRouting without opening a device. */
  ComediRouting( const string &deviceclass );
    /*! Open comedi specified by its device file \a device. */
  ComediRouting( const string &device, const Options &opts );
    /*! Close the comedi driver. */
  virtual ~ComediRouting( void );

    /*! Open the device specified by \a device
        and set the routing as specified by opts.
 	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device) override;
    /*! \return \c true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void );


protected:

    /*! Set routing of signal \a routing to channel \a channel on
        subdevice \a subdev and configure channel as output.  \a
        signal is the optional name of the routing signal that is
        displayed in the devices informations. */
  int open( const string &device, int subdev, int channel,
	    int routing, const string &signal="" );

  virtual void initOptions( void ) override;

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDIROUTING_H_ */
