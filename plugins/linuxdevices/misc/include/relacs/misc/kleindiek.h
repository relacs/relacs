/*
  misc/kleindiek.h
  The Kleindiek nanotechnik MM3A micromanipulator  

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

  virtual int open( const string &device )  override;
  virtual bool isOpen( void ) const { return Handle >= 0; };
  virtual void close( void );
  virtual int reset( void );

    /*! Immediately stop movement of axis \a axis. */
  virtual int stop( int axis ) { return NotSupported; };

    /*! Set the amplitude of a step of the axis \a axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setStepAmpl( int axis, double posampl, double negampl=-1.0 );

    /*! The minimum possible amplitude for the x-axis. */
  virtual double minAmplX( void ) const;
    /*! The maximum possible amplitude for the x-axis. */
  virtual double maxAmplX( void ) const;

  friend ostream &operator<<( ostream &str, const Kleindiek &k );

    /*! Set the speed for channle \a channel to \a speed.
        Valid values are integers from 1 to 6. */
  int speed( int channel, int speed );
  int amplitudepos( int channel, int ampl );
  int amplitudeneg( int channel, int ampl );

  int countermode( int channel, long mode );
  int counterread( void );
  int counterreset( void );

    /*! Step axis \a axis by \a steps spteps. */
  int doStepBy( int axis, int steps, double speed=0, double acc=0 );


protected:

  int pause( int ms );


private:

  void construct( void );

  int Handle;
  struct termios OldTIO;
  struct termios NewTIO;

  int PosAmplitude[3];
  int NegAmplitude[3];
  double PosGain[3];  // distance of single, positive full amplitude step in um
  double NegGain[3];  // distance of single, negative full amplitude step in um

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_KLEINDIEK_H_ */
