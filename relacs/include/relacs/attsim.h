/*
  attsim.h
  Implementation of the Attenuator class

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

#ifndef _ATTSIM_H_
#define _ATTSIM_H_

#include <relacs/attenuator.h>

/*! 
\class AttSim
\author Jan Benda
\version 1.1
\brief Implementation of the Attenuator class
for simulating an attenuator.


The attenuation levels are stored in the global variable
\a AttSimDecibel.
*/


class AttSim: public Attenuator
{

public:

    /*! Constructs an AttSim. */
  AttSim( void );
    /*! Close the attenuator simulation. */
  ~AttSim( void );

    /*! Open the attenuator device driver specified by \a device. */
  int open( const string &device, long mode=0 );
    /*! Returns true if the attenuator device driver was succesfully opened. */
  bool isOpen( void ) const;
    /*! Close the attenuator device driver. */
  void close( void );

    /*! Returns a string with the current settings of the attenuator. */
  virtual string settings( void ) const;

    /*! Returns the number of attenuator devices the driver handles. */
  int lines( void ) const;

    /*! Set the attenuation level of the subdevice specified by its index \a di
        to \a decibel decibel. 
        Returns the actually set level in \a decibel. */
  int attenuate( int di, double &decibel );
    /*! Tests setting the attenuation level of the subdevice specified by its index \a di
        to \a decibel decibel. 
        Returns the actually set level in \a decibel. */
  int testAttenuate( int di, double &decibel );


  static const int MaxDevices = 10;
  static double Decibel[MaxDevices];


private:

  static const double AttStep;
  static const double AttMax;
  static const double AttMin;

};

#endif
