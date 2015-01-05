/*
  calibration/restartdelay.h
  Measures delays due to restart of the data acquisition

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

#ifndef _RELACS_CALIBRATION_RESTARTDELAY_H_
#define _RELACS_CALIBRATION_RESTARTDELAY_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace calibration {


/*!
\class RestartDelay
\brief [RePro] Measures delays due to restart of the data acquisition
\author Jan Benda
\version 1.4 (Feb 18, 2014)

\par Options
\par Options
- \c duration=100ms: Duration of analysis window (\c number)
- \c repeats=100: Repeats (\c integer)
- \c intrace=V-1: Input trace (\c string)

\par Files
\arg No output files.

\par Plots
\arg The input sine wave (green) with the fits (red).

\par Requirements
\arg A sine wave generator connected to the selected input channel.
*/


class RestartDelay : public RePro
{
  Q_OBJECT

public:

  RestartDelay( void );
  virtual void preConfig( void );
  virtual int main( void );
  int analyze( const InData &data, double duration, int count, double &deltat );

protected:

  Plot P;

};


}; /* namespace calibration */

#endif /* ! _RELACS_CALIBRATION_RESTARTDELAY_H_ */
