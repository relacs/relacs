/*
  base/envelope.h
  Computes the envelope of a signal

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

#ifndef _RELACS_BASE_ENVELOPE_H_
#define _RELACS_BASE_ENVELOPE_H_ 1

#include <relacs/filter.h>
using namespace relacs;

namespace base {


/*!
\class Envelope
\brief [Filter] Computes the envelope of a signal
\author Jan Benda
\version 1.0 (Jun 14, 2012)


Add the low-pass filter with the following lines to a \c relacs.cfg %file:
\verbatim
*FilterDetectors
  Filter1
        name: AM-1
      filter: Envelope
  inputtrace: V-1
        save: false
        plot: true
  buffersize: 500000
\endverbatim

*/


class Envelope : public Filter
{
  Q_OBJECT

public:

  Envelope( const string &ident="", int mode=0 );
  virtual int init( const InData &indata, InData &outdata  );
  virtual int adjust( const InData &indata, InData &outdata );
  virtual void notify( void );
  virtual int filter( const InData &indata, InData &outdata );


protected:

  OptWidget EFW;

  bool DeMean;
  double MeanTau;
  int Rectification;
  double Tau;

  double DeltaT;
  double TFac;
  InDataIterator Index;
  float X;

  double MeanTFac;
  float Mean;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_ENVELOPE_H_ */
