/*
  auditoryprojects/fiphaselocking.h
  Phase locking at different carrier frequencies and firing rates.

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

#ifndef _RELACS_AUDITORYPROJECTS_FIPHASELOCKING_H_
#define _RELACS_AUDITORYPROJECTS_FIPHASELOCKING_H_ 1

#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace auditoryprojects {


/*!
\class FIPhaseLocking
\brief [RePro] Phase locking at different carrier frequencies and firing rates.
\author Karin Fisch
\version 1.0 (Feb 01, 2010)
*/


class FIPhaseLocking : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  FIPhaseLocking( void );
  virtual int main( void );

protected:

  MultiPlot P;

  struct Data
  {
    double CarrierFrequency;
    double Intensity;
    EventList Spikes;
  };
  vector< Data > Results;

private:
  
  void saveSpikes( void );
  void plot( int );

  double duration;

};


}; /* namespace auditoryprojects */

#endif /* ! _RELACS_AUDITORYPROJECTS_FIPHASELOCKING_H_ */
