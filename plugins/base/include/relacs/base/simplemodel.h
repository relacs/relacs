/*
  base/simplemodel.h
  A toy model for testing.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

Records either Gaussian white noise (\c model=Noise),
a sine wave with the specified frequency (\c model=Sine),
or the stimulus (\c model=Stimulus), multiplied by \c gain.

\par Options
- \c model=Noise: The model (\c string)
- \c gain=1: Gain (\c number)
- \c freq=1000Hz: Frequency (\c number)

\version 1.1 (Jan 31, 2008)
*/


class SimpleModel : public Model
{

public:

  SimpleModel( void );
  virtual ~SimpleModel( void );

  virtual void main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SIMPLEMODEL_H_ */
