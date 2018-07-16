/*
  auditory/search.h
  Periodically emits a search stimulus.

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

#ifndef _RELACS_AUDITORY_SEARCH_H_
#define _RELACS_AUDITORY_SEARCH_H_ 1


#include <relacs/lcdrange.h>
#include <relacs/repro.h>
#include <relacs/acoustic/traces.h>
#include <QPushButton>
#include <QRadioButton>
using namespace relacs;

namespace auditory {


/*! 
\class Search
\brief [RePro] Periodically emits a search stimulus.
\author Jan Benda
\author Christian Machens
\version 2.8 (July 16, 2018)

\par Options
\c Stimulus
- \c intensity=80dB SPL: Sound intensity (\c number)
- \c minintensity=0dB SPL: Minimum sound intensity (\c number)
- \c maxintensity=120dB SPL: Maximum sound intensity (\c number)
- \c mute=false: No stimulus (\c boolean)
- \c frequency=5kHz: Frequency of stimulus (\c number)
- \c minfreq=0kHz: Minimum allowed frequency (\c number)
- \c maxfreq=1000kHz: Maximum allowed frequency (\c number)
- \c waveform=sine: Waveform of stimulus (\c string)
- \c ramp=2ms: Ramp (\c number)
\c Settings
- \c side=left: Speaker (\c string)
- \c repeats=0: Number of repetitions (\c integer)
- \c duration=50ms: Duration of stimulus (\c number)
- \c pause=500ms: Duration of pause (\c number)
- \c prepause=50ms: Part of pause before stimulus (\c number)
- \c adjust=true: Adjust input gains (\c boolean)
- \c saving=true: Save raw data (\c boolean)
- \c setbestside=never: Set the sessions's best side (\c string)
- \c keep=true: Keep changes (\c boolean)

\par Keyboard shortcuts:
- \c Left: Left speaker
- \c Right: Right speaker
- \c X, \c %Pause: Toggle stimulus output
- \c Up: Increase intensity by 1dB
- \c Down: Decrease intensity by 1dB
- \c Shift+Up: Increase intensity by 5dB
- \c Shift+Down: Decrease intensity by 5dB
- \c Ctrl+Up: Increase frequency by 1kHz
- \c Ctrl+Down: Decrease frequency by 1kHz
- \c Ctrl+Shift+Up: Increase frequency by 5kHz
- \c Ctrl+Shift+Down: Decrease frequency by 5kHz
- \c Ctrl+Left: Decrease duration of pause by 5ms
- \c Ctrl+Right: Increase duration of pause by 5ms
- \c Ctrl+Shift+Left: Decrease duration of pause by 50ms
- \c Ctrl+Shift+Right: Increase duration of pause by 50ms
- \c Alt+N: Band limited white noise
- \c Alt+S: Sine wave (pure tone)
 */


class Search : public RePro, public acoustic::Traces
{

  Q_OBJECT

public:

  Search( void );
  ~Search( void );

  virtual int main( void );

    /*! Save event data. */
  void saveEvents( const EventData &events, int count, Options &header );


public slots:

    /*! Set intensity of stimulus to \a dB dB and emit IntensityChanged. */
  void setIntensity( double intensity );
    /*! Set duration of stimulus to \a noise ms and emit DurationChanged.*/
  void setDuration( double duration );
    /*! Set duration of pause to \a pause ms and emit PauseChanged. */
  void setPause( double pause );
    /*! Set frequency of stimulus to \a freq Hz and emit FrequencyChanged. */
  void setFrequency( double freq );
    /*! Set waveform of stimulus and emit waveformChanged.*/
  void setWaveform( int wave );
    /*! Set the right waveform button.*/
  void setWaveformButton( int wave );
    /*! Set output speaker. */
  void setSpeaker( bool left );
    /*! Set output speaker to "left". */
  void setSpeakerLeft( void );
    /*! Set output speaker to "right". */
  void setSpeakerRight( void );
    /*! Toggle mute mode. */
  void toggleMute( void );
    /*! Set mute mode. */
  void setMute( bool mute );

    /*! Updates all input widgets to the values of the options. */
  virtual void notify( void );


protected:

    /*! Handle keyboard events. */
  void keyPressEvent( QKeyEvent *qke );
  virtual void customEvent( QEvent *qce );

  bool NewSignal;
  int SetBestSide;

  // values of search variables

  // search variables and their tuning:
  bool SearchLeft;                           // ouput channel
  QRadioButton *LeftButton;
  QRadioButton *RightButton;

  bool Mute;
  QPushButton *MuteButton;

  LCDRange *ILCD;
  double Intensity;                       // (in dB)
  double PrevIntensity;
  double MaxIntensity;
  double MinIntensity;  
  static const double ShortIntensityStep;
  static const double LongIntensityStep;

  LCDRange *DLCD;
  double Duration;                         // in seconds
  static const double ShortDurationStep;
  static const double LongDurationStep;
  static const double MaxDuration;
  static const double MinDuration;

  LCDRange *PLCD;
  double Pause;                         // in seconds
  double PrePause;
  static const double ShortPauseStep;
  static const double LongPauseStep;
  static const double MaxPause;
  static const double MinPause;

  LCDRange *FLCD;
  double Frequency;    // in Hz
  double MinFrequency; // in Hz
  double MaxFrequency; // in Hz
  static const double ShortFrequencyStep;
  static const double LongFrequencyStep;

  int Waveform;
  QRadioButton *SineButton;
  QRadioButton *NoiseButton;

  double Ramp;

  bool KeepChanges;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SEARCH_H_ */
