/*
  auditoryprojects/mothasongs.h
  Artificial songs of Arctiid moths

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

#ifndef _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_
#define _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace auditoryprojects {


/*!
\class MothASongs
\brief [RePro] Artificial songs of Arctiid moths
\author Nils Brehm
\version 1.0 (Sep 29, 2017)
\par Options
- \c Stimulus
    - \c duration=110ms: Stimulus duration (\c number)
    - \c intensity=80dB SPL: Intensity (\c number)
    - \c repeats=1: Repeats (\c integer)
    - \c pause=1000ms: Pause (\c number)
    - \c side=left: Speaker (\c string)
- \c Pulse settings
    - \c tau=1ms: Damping time-scale (\c number)
    - \c apulserange="0..40..10"ms: Active pulse times (\c string)
    - \c ppulserange="60..100..10"ms: Passive pulse times (\c string)
    - \c afreq=10kHz: Active pulse frequencies (\c string)
    - \c pfreq=10kHz: Passive pulse Frequencies (\c string)
    - \c samplingrate=200kHz: Sampling rate (\c number)
- \c Analysis
    - \c before=100ms: Time before stimulus to be analyzed (\c number)
    - \c after=100ms: Time after stimulus to be analyzed (\c number)
    - \c sigma=1ms: Standard deviation of rate smoothing kernel (\c number)
*/


class MothASongs : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  MothASongs( void );
  virtual int main( void );

protected:

  void saveSpikes( const Options &header, const EventList &spikes );
  void saveRate( const Options &header, const SampleDataD &rate );
  void saveStimulus( const Options &header, const OutData &wave );

  MultiPlot P;

};


}; /* namespace auditoryprojects */

#endif /* ! _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_ */
