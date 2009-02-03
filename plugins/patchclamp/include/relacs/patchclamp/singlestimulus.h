/*
  patchclamp/singlestimulus.h
  Output of a single stimulus stored in a file.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_
#define _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ 1

#include <relacs/repro.h>
#include <relacs/eventlist.h>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*! 
\class SingleStimulus
\brief [RePro] Output of a single stimulus stored in a file.
\author Jan Benda
\version 1.2 (Oct 13, 2008)
*/


class SingleStimulus : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  SingleStimulus( void );
    /*! Destructor. */
  ~SingleStimulus( void );

  virtual void config( void );
  virtual void notify( void );
  void readConfig( StrQueue &sq );

  virtual int main( void );

  void saveSpikes( Options &header, const EventList &spikes );
  void saveRate( Options &header, const SampleDataD &rate1,
		 const SampleDataD &rate2 );
  void save( const EventList &spikes, const SampleDataD &rate1,
	     const SampleDataD &rate2 );

    /*! Plot data. */
  void plot( const EventList &spikes, const SampleDataD &rate1,
	     const SampleDataD &rate2, const OutData &signal );
    /*! Analyze data. */
  void analyze( EventList &spikes, SampleDataD &rate1, SampleDataD &rate2 );


protected:

  int createStimulus( OutData &signal, const Str &file,
		      double &duration, double deltat, bool storesignal );

  void customEvent( QCustomEvent *qce );

  string AmplitudeUnit;
  double Amplitude;
  double PeakAmplitude;
  double PeakAmplitudeFac;
  enum WaveForms { File=0, Const, Sine, Rectangular, Triangular,
		   Sawup, Sawdown, Whitenoise, OUnoise };
  WaveForms WaveForm;
  double Frequency;
  double DutyCycle;
  int Seed;
  double Ramp;
  double Offset;
  double Duration;
  int Side;
  int Repeats;
  double SkipWin;
  double Sigma1;
  double Sigma2;

  string StimulusLabel;
  enum StoreModes { SessionPath, ReProPath, CustomPath };
  enum StoreLevels { All, Generated, Noise, None };
  StoreLevels StoreLevel;
  Str StorePath;
  Str StoreFile;
  double MeanRate;
  SampleDataD Rate1;
  SampleDataD Rate2;

  MultiPlot SP;
  MultiPlot P;

  Options Settings;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ */
