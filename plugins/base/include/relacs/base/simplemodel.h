/*
  base/simplemodel.h
  A toy model for testing.

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

#ifndef _RELACS_BASE_SIMPLEMODEL_H_
#define _RELACS_BASE_SIMPLEMODEL_H_ 1

#include <relacs/model.h>
using namespace relacs;

namespace base {


/*! 
\class SimpleModel
\brief [Model] A toy model for testing.
\author Jan Benda
\version 1.2 (Oct 10, 2013)


Records the stimulus with some Gaussian white noise and a sine wave
added to it. The amplitude of each component can be scaled with the
respective \a gain option. In particular, components can be disabled by 
setting their \a gain to zero.

\par Options
- \c stimulusgain=1: Gain of stimulus (\c number)
- \c noisegain=0: Amplitude of white noise (\c number)
- \c sinegain=0: Amplitude of sine wave (\c number)
- \c sinefreq=1000Hz: Frequency of sine wave (\c number)
*/


class SimpleModel : public Model
{

public:

  SimpleModel( void );
  virtual ~SimpleModel( void );

  virtual void preConfig( void );
  virtual void main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SIMPLEMODEL_H_ */
