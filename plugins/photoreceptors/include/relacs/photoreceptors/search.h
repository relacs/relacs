/*
  photoreceptors/search.h
  Search repro giving current and light pulses on two analog outs.

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

#ifndef _RELACS_PHOTORECEPTORS_SEARCH_H_
#define _RELACS_PHOTORECEPTORS_SEARCH_H_ 1

#include <QPushButton>
#include <relacs/base/traces.h>
#include <relacs/ephys/traces.h>
#include <relacs/tablekey.h>
#include <relacs/repro.h>
using namespace relacs;

namespace photoreceptors {


/*!
\class Search
\brief [RePro] Search repro giving current and light pulses on two analog outs.
\author Jan Grewe
\version 1.0 (May 06, 2015)
*/


class Search : public RePro, public base::Traces, public ephys::Traces
{
  Q_OBJECT

public:

  Search( void );
  virtual int main( void );

private:
  OptWidget jw;
  QPushButton *startButton;
  QPushButton *stopButton;
  string Vunit;
  bool Start;
  
  void populateOptions( void );
  void createLayout( void );
  SampleDataF createPulse( double total_duration, double pulse_duration, double offset,
			   double sample_rate, double intensity = 1.0 );
  void openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index, const SampleDataF &voltage );
};


}; /* namespace photoreceptors */

#endif /* ! _RELACS_PHOTORECEPTORS_SEARCH_H_ */
