/*
  base/lowpass.h
  A simple first order low pass filter

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

#ifndef _RELACS_BASE_LOWPASS_H_
#define _RELACS_BASE_LOWPASS_H_ 1

#include <relacs/optwidget.h>
#include <relacs/indata.h>
#include <relacs/filter.h>
using namespace relacs;

namespace base {


/*! 
\class LowPass
\brief [Filter] A simple first order low pass filter
\author Jan Benda

The input \a x(t) is filtered with the ordinary differential equation
\f[ \tau \frac{dy}{dt} = x - y \f]
to result in the low-pass filtered output \a y(t).
The cut-off frequency of the filter is at
\f[ f_c = \frac{1}{2 \pi \tau} \f]
 
\par Options
- \c tau=1000ms: Filter time constant (\c number)

\version 0.1 (May 3 2007)
*/


class LowPass : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  LowPass( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~LowPass( void );

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

#endif /* ! _RELACS_BASE_LOWPASS_H_ */
