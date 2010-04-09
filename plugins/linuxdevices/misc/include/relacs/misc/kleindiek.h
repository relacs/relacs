/*
  misc/kleindiek.h
  The Kleindiek nanotechnik MM3A micromanipulator  

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

#ifndef _RELACS_MISC_KLEINDIEK_H_
#define _RELACS_MISC_KLEINDIEK_H_ 1

#include <string>
#include <vector>
#include <termios.h>
#include <relacs/manipulator.h>
using namespace std;
using namespace relacs;

namespace misc {


/*!
\class Kleindiek
\author Jan Benda
\version 1.0
\brief [Manipulator] The %Kleindiek nanotechnik MM3A micromanipulator
*/

class Kleindiek : public Manipulator
{

public:

  Kleindiek( const string &device );
  Kleindiek( void );
  ~Kleindiek( void );

  virtual int open( const string &device, const Options &opts );
  virtual bool isOpen( void ) const { return Handle >= 0; };
  virtual void close( void );
  virtual int reset( void );

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

    /*! Set the amplitude of a step of the x-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplX( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the y-axis to \a posampl.
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

  friend ostream &operator<<( ostream &str, const Kleindiek &k );

  int coarse( int channel, int steps );
  int pause( int ms );

  int speed( int channel, int speed );
  int amplitudepos( int channel, int ampl );
  int amplitudeneg( int channel, int ampl );

  int countermode( int channel, long mode );
  int counterread( void );
  int counterreset( void );


private:

  void construct( void );

  int Handle;
  struct termios OldTIO;
  struct termios NewTIO;

  double Pos[3];
  int PosAmplitude[3];
  int NegAmplitude[3];
  double PosGain[3];  // distance of single, positive full amplitude step in um
  double NegGain[3];  // distance of single, negative full amplitude step in um

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_KLEINDIEK_H_ */
