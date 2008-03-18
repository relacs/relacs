/*
  manipulator.h
  Virtual class for controlling a Manipulator. 

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _MANIPULATOR_H_
#define _MANIPULATOR_H_


#include <relacs/device.h>
using namespace std;


/*!
\class Manipulator
\brief Virtual class for controlling a Manipulator. 
\author Jan Benda
\version 1.0

The Manipulator class defines an interface for basic operations
to control a manipulator. 
*/
  

class Manipulator : public Device
{

    /*! Device type id for Manipulator devices. */
  static const int Type = 5;


public:

    /*! Construct a Manipulator. */
  Manipulator( void );
    /*! Construct a Manipulator with class \a deviceclass.
        \sa setDeviceClass() */
  Manipulator( const string &deviceclass );
    /*! Destroy a Manipulator. In case it is open, close it. */
  virtual ~Manipulator( void );

    /*! Move x-axis by \a x.
        Depending on the implementation \a x can be raw steps
	or a specific distance or angle. */
  virtual int stepX( double x );
    /*! Move y-axis by \a y.
        Depending on the implementation \a y can be raw steps
	or a specific distance or angle. */
  virtual int stepY( double y );
    /*! Move z-axis by \a z.
        Depending on the implementation \a z can be raw steps
	or a specific distance or angle. */
  virtual int stepZ( double z );

    /*! Defines the current position of the x axis as the home position. */
  virtual int clearX( void );
    /*! Defines the current position of the y axis as the home position. */
  virtual int clearY( void );
    /*! Defines the current position of the z axis as the home position. */
  virtual int clearZ( void );
    /*! Defines the current position of all axis as the home position. */
  virtual int clear( void );

    /*! Move x axis back to its home position. */
  virtual int homeX( void );
    /*! Move y axis back to its home position. */
  virtual int homeY( void );
    /*! Move z axis back to its home position. */
  virtual int homeZ( void );
    /*! Move back to the home position. */
  virtual int home( void );

    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posX( void ) const;
    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posY( void ) const;
    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posZ( void ) const;

    /*! Set the amplitude of a step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplX( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplY( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplZ( double posampl, double negampl=-1.0 );

    /*! The minimum possible amplitude for the x-axis. */
  virtual double minAmplX( void ) const;
    /*! The maximum possible amplitude for the x-axis. */
  virtual double maxAmplX( void ) const;
    /*! The minimum possible amplitude for the y-axis. */
  virtual double minAmplY( void ) const;
    /*! The maximum possible amplitude for the y-axis. */
  virtual double maxAmplY( void ) const;
    /*! The minimum possible amplitude for the z-axis. */
  virtual double minAmplZ( void ) const;
    /*! The maximum possible amplitude for the z-axis. */
  virtual double maxAmplZ( void ) const;

};

#endif
