/*
  tempdtm5080.h
  

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

#ifndef _RELACS_HARDWARE_TEMPDTM5080_H_
#define _RELACS_HARDWARE_TEMPDTM5080_H_ 1

#include <string>
#include <vector>
#include <termios.h>
#include <relacs/temperature.h>
using namespace std;
using namespace relacs;


class TempDTM5080 : public Temperature
{

public:

  TempDTM5080( void );
  TempDTM5080( const string &device, long probe );
  ~TempDTM5080( void );

  virtual int open( const string &device, long probe=1 );
  virtual bool isOpen( void ) const;
  virtual void close( void );
  virtual int reset( void );
  virtual string info( void ) const;
  virtual string settings( void ) const;

  virtual double temperature( void );

  void setProbe( int probe );

  friend ostream &operator<<( ostream &str, const TempDTM5080 &k );


private:

  int Handle;
  int Probe;
  struct termios OldTIO;
  struct termios NewTIO;

};


#endif /* ! _RELACS_HARDWARE_TEMPDTM5080_H_ */
