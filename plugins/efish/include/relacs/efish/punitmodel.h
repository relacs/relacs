/*
  efish/punitmodel.h
  A model for P-units of weakly-electric fish.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_EFISH_PUNITMODEL_H_
#define _RELACS_EFISH_PUNITMODEL_H_ 1

#include <relacs/ephys/neuronmodels.h>
#include <relacs/efield/traces.h>
using namespace relacs;

namespace efish {


/*!
\class PUnitModel
\brief [Model] A model for P-units of weakly-electric fish.
\author Jan Benda
\version 1.0 (Nov 27, 2009)
*/


class PUnitModel : public ephys::NeuronModels, public efield::Traces
{

public:

  PUnitModel( void );
  ~PUnitModel( void );

  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );
  virtual void operator()( double t, double *x, double *dxdt, int n );


 protected:

  double EODFreq;
  double EODFreqSD;
  double EODFreqTau;
  double EODFreqFac;
  double EODAmpl1;
  double EODAmpl2;
  double SignalFac;
  double VoltageScale;

  double Signal;
  double EOD1;
  double EOD2;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_PUNITMODEL_H_ */
