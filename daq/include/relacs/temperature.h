/*
  temperature.h
  Virtual class for reading a temperature. 

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

#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "device.h"
using namespace std;


/*!
\class Temperature
\brief Virtual class for reading a temperature. 
\author Jan Benda
\version 1.0
*/


class Temperature : public Device
{

    /*! Device type id for Temperature devices. */
  static const int Type = 6;


public:

    /*! Construct a Temperature Device. */
  Temperature( void );
    /*! Construct a Temperature Device with class \a deviceclass.
        \sa setDeviceClass() */
  Temperature( const string &deviceclass );

    /*! The current temperature in degree celsius. */
  virtual double temperature( void ) = 0;

};


#endif
