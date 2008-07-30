/*
  patchclamp/simple.h
  Simple RePro for testing dynamic clamp

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

#ifndef _RELACS_PATCHCLAMP_SIMPLE_H_
#define _RELACS_PATCHCLAMP_SIMPLE_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace patchclamp {


/*!
\class Simple
\brief [RePro] Test dynamic clamp
\author Jan Benda
\version 0.0 (Feb 14, 2008)

\par Options
\arg \b intrace (\c text): Name of the input trace. 
\arg \b outtrace (\c text): Name of the output trace. 
\arg \b amplitude (\c number): Amplitude of output signal.
\arg \b duration (\c number, \e ms): Duration of stimulus.
\arg \b pause (\c number, \e ms): Duration of pause bewteen outputs.
\arg \b repeats (\c integer): Number of repetitions. 

\par Files
\arg No output files.

\par Plots
\arg 

\par Requirements
\arg 
*/


class Simple : public RePro
{
  Q_OBJECT

public:

  Simple( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );

protected:

  Plot P;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SIMPLE_H_ */

