/*
  voltageclamp/pnsubtraction.h
  P-N Subtraction for removing stimulus artifacts in voltageclamp recordings

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

#ifndef _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_
#define _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_ 1

#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace voltageclamp {


/*!
\class PNSubtraction
\brief [RePro] P-N Subtraction for removing stimulus artifacts in voltageclamp recordings
\author Lukas Sonnenberg
\version 1.0 (Oct 08, 2018)
*/


class PNSubtraction : public RePro, public ephys::Traces
{
  Q_OBJECT

friend class Activation;

public:

  PNSubtraction( const string &name,
	 const string &pluginset="",
	 const string &author="unknown",
	 const string &version="unknown",
	 const string &date=__DATE__ );
  //PNSubtraction( void );
  virtual int main( void );

  SampleDataD PN_sub( OutData signal, Options &opts, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0);

private:
	double linearFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp );
//	void analyzeCurrentPulse( SampleDataD voltagetrace, double I0 );
//	ArrayD pcsFitLeak( SampleDataD &potentialtrace, SampleDataD &currenttrace, double stepduration );

	double EL;
	double gL;
	double Cm;
	double tau;

};


}; /* namespace voltageclamp */

#endif /* ! _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_ */
