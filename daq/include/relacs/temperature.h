/*
  temperature.h
  Virtual class for reading a temperature. 

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

#ifndef _RELACS_TEMPERATURE_H_
#define _RELACS_TEMPERATURE_H_ 1

#include <relacs/device.h>
using namespace std;

namespace relacs {


/*!
\class Temperature
\brief Virtual class for reading a temperature. 
\author Jan Benda
\version 1.0
*/


class Temperature : public Device
{

public:

    /*! Construct a Temperature Device. */
  Temperature( void );
    /*! Construct a Temperature Device with class \a deviceclass.
        \sa setDeviceClass() */
  Temperature( const string &deviceclass );

    /*! The current temperature in degree celsius.
        The reimplementation of this function
        should call Settings.setNumber( "temperature", x );
        where \a x is the currently measured temperature. */
  virtual double temperature( void ) = 0;

};


}; /* namespace relacs */

#endif /* ! _RELACS_TEMPERATURE_H_ */
