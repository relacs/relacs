/*
  photoreceptors/twochannelstimulus.h
  Puts out two stimuli on two output channels

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

#ifndef _RELACS_PHOTORECEPTORS_TWOCHANNELSTIMULUS_H_
#define _RELACS_PHOTORECEPTORS_TWOCHANNELSTIMULUS_H_ 1

#include <relacs/base/traces.h>
#include <relacs/ephys/traces.h>
#include <relacs/tablekey.h>
#include <relacs/repro.h>
using namespace relacs;

namespace photoreceptors {


/*!
\class TwoChannelStimulus
\brief [RePro] Puts out two stimuli on two output channels
\author Jan Grewe
\version 1.0 (Jun 02, 2015)
*/


  class TwoChannelStimulus : public RePro, public base::Traces, public ephys::Traces
  {
    Q_OBJECT
    
  public:
    
    TwoChannelStimulus( void );
    virtual int main( void );
  
    enum WaveForms { Const=0, Sine, Rectangular, Whitenoise };
    enum StoreModes { SessionPath, ReProPath, CustomPath };
  
  private:
    
    void preConfig();
    double checkChannelOffset(double offset, int offsetbase, double amplitude, double current_dc);
    bool checkStimulusDuration(double duration, double skipwin);
    void checkPulseDuration(int pulse_selection, double &duty_cycle, double &pulse_duration, double frequency);
    double checkFrequency(int freq_selection, double frequency, double period, int num_periods, double duration);
    Options createStimulus( WaveForms waveform, OutData &signal, double &duration, double deltat, double ramp,
			    double cycleramp, double pulse_duration, double duty_cycle, 
			    double frequency, int seed, double amplitude);
    string writeStimulusFile(const OutData &signal, Options &header, const string &path, const string &channel_prefix);
    void openTraceFile(ofstream &tf,  TableKey &tracekey, const Options &header);
    void saveTrace( ofstream &tf, TableKey &tracekey, int index, const SampleDataF &voltage );

    WaveForms ch1_waveform, ch2_waveform;
    Str StorePath;
    Str StoreFile;
    string i_unit, v_unit;
    double i_in_factor;
    
  };
  

}; /* namespace photoreceptors */

#endif /* ! _RELACS_PHOTORECEPTORS_TWOCHANNELSTIMULUS_H_ */
