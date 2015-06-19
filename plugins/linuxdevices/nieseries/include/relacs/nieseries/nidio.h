/*
  nieseries/nidio.h
  Controls the digital I/O lines of a National Instruments E Series board.

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

#ifndef _RELACS_NIESERIES_NIDIO_H_
#define _RELACS_NIESERIES_NIDIO_H_ 1

#include <string>
#include <relacs/device.h>
using namespace std;
using namespace relacs;

namespace nieseries {


/*!
\class NIDIO
\author Jan Benda
\version 1.0
\brief [Device] Controls the digital I/O lines of a National Instruments E Series board.

\par Options
- \c config: Configure the digital I/O pins for input (0) or output (1).
 */

class NIDIO : public Device
{

public:

  NIDIO( const string &device );
  NIDIO( void );
  ~NIDIO( void );

  virtual int open( const string &device ) override;
  virtual bool isOpen( void ) const { return Handle >= 0; };
  virtual void close( void );

    /*! Allocate the pins \a pins of the DIO lines. 
        Returns the pins that are already allocated. */
  int allocPins( int pins );
    /*! Free the pins \a pins. */
  void freePins( int pins );

    /*! Configure DIO lines specified by \a mask for input (0) or output (1). */
  int configure( int dios, int mask ) const;
    /*! Write \a dios to the DIO lines defined in \a mask . */
  int out( int dios, int mask );
    /*! Read DIO lines and return them in \a dios. */
  int in( int &dios ) const;

    /*! Set the EXTSTROBE/SDCLK pin to \a high. */
  int setClock( bool high );

    /*! Specify timing for serial IO.
        timing table (clock cycle length in microseconds):
                        div2:
            slow:       0          1
    
               0        0.6        1.2
    
               1        5.0        5.0
    */
  int setTiming( bool div2, bool slow ) const;
    /*! Write \a buf to serial out on DIO 0 */
  int write( unsigned char *buf, int n );
    /*! Read \a buf from serial in on DIO 4 */
  int read( unsigned char *buf, int n ) const;


private:

    /*! Handle for the device. */
  int Handle;

  int Pins;

};


}; /* namespace nieseries */

#endif /* ! _RELACS_NIESERIES_NIDIO_H_ */
