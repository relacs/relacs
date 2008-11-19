/*
  base/highpass.h
  A simple first order high pass filter

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

#ifndef _RELACS_BASE_HIGHPASS_H_
#define _RELACS_BASE_HIGHPASS_H_ 1

#include <relacs/optwidget.h>
#include <relacs/indata.h>
#include <relacs/filter.h>
using namespace relacs;

namespace base {


/*! 
\class HighPass
\brief [Filter] A simple first order high pass filter
\author Jan Benda

\par Options
- \c tau=1ms: Time constant (\c number)

\version 0.1 (Jan 24 2008)
*/


class HighPass : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  HighPass( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~HighPass( void );

  virtual int init( const InData &indata, InData &outdata );
  virtual void notify( void );
  virtual int filter( const InData &indata, InData &outdata );


protected:

  OptWidget LFW;

  double Tau;

  double DeltaT;
  double TFac;
  InDataIterator Index;
  float X;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_HIGHPASS_H_ */
