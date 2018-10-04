/*
  efish/punitmodel.h
  A model for P-units of weakly-electric fish.

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
\version 2.0 (Oct 30, 2013)
\par Options
- \c General
    - \c EOD
        - \c eodtype=Sine: EOD type (\c string)
        - \c eodfreq=800Hz: Frequency (\c number)
        - \c eodfreqsd=10Hz: SD of frequency (\c number)
        - \c eodfreqtau=1000s: Timescale of frequency (\c number)
        - \c eodlocalamplitude=1mV/cm: Amplitude for local electrode (\c number)
        - \c eodglobalamplitude=1mV/cm: Amplitude for global electrode (\c number)
        - \c localstimulusgain=1: Gain for additive stimulus component to local electrode (\c number)
        - \c globalstimulusgain=0: Gain for additive stimulus component to global electrode (\c number)
        - \c stimulusgain=1: Gain for stimulus recording channel (\c number)
    - \c Spikes
        - \c voltagescale=1: Scale factor for membrane potential (\c number)
- \c Spike generator
    - \c spikemodel=Stimulus: Spike model (\c string)
    - \c noised=0: Intensity of current noise (\c number)
    - \c deltat=0.005ms: Delta t (\c number)
    - \c integrator=Euler: Method of integration (\c string)
- \c Voltage-gated current 1 - activation only
    - \c gmc=0: Conductivity (\c number)
    - \c emc=-90mV: Reversal potential (\c number)
    - \c mvmc=-40mV: Midpoint potential of activation (\c number)
    - \c mwmc=10mV: Width of activation (\c number)
    - \c taumc=10ms: Time constant (\c number)
- \c Voltage-gated current 2 - activation and inactivation
    - \c gmhc=0: Conductivity (\c number)
    - \c emhc=-90mV: Reversal potential (\c number)
    - \c mvmhc=-40mV: Midpoint potential of activation (\c number)
    - \c mwmhc=10mV: Width of activation (\c number)
    - \c taummhc=10ms: Time constant of activation (\c number)
    - \c pmmhc=1: Power of activation gate (\c number)
    - \c hvmhc=-40mV: Midpoint potential of inactivation (\c number)
    - \c hwmhc=10mV: Width of inactivation (\c number)
    - \c tauhmhc=10ms: Time constant of inactivation (\c number)
    - \c pmhhc=1: Power of inactivation gate (\c number)
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

  int EODType;
  double EODFreq;
  double EODFreqSD;
  double EODFreqTau;
  double EODFreqFac;
  double EODLocalAmplitude;
  double EODGlobalAmplitude;
  double LocalStimulusGain;
  double GlobalStimulusGain;
  double StimulusGain;
  double VoltageScale;

  double Signal;
  double LocalSignal;
  double EODLocal;
  double EODGlobal;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_PUNITMODEL_H_ */
