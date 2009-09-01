/*
  auditory/search.h
  Periodically emits a search stimulus.

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

#ifndef _RELACS_AUDITORY_SEARCH_H_
#define _RELACS_AUDITORY_SEARCH_H_ 1


#include <relacs/repro.h>
#include <relacs/acoustic/traces.h>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
using namespace relacs;

namespace auditory {


/*! 
\class Search
\brief [RePro] Periodically emits a search stimulus.
\author Jan Benda
\author Christian Machens
\todo Use mutex to protect stimulus creation.
\bug sampling rate is only 10 kHz?!
\todo reanable manual convert of signal

\par Options
- \c intensity=80dB: Intensity (\c number)
- \c mute=false: No stimulus (\c boolean)
- \c duration=50ms: Duration of stimulus (\c number)
- \c pause=500ms: Duration of pause (\c number)
- \c prepause=50ms: Part of pause before stimulus (\c number)
- \c frequency=5kHz: Frequency of stimulus (\c number)
- \c waveform=sine: Waveform of stimulus (\c string)
- \c ramp=2ms: Ramp (\c number)
- \c side=left: Speaker (\c string)
- \c repeats=0: Number of repetitions (\c integer)
- \c adjust=true: Adjust input gains (\c boolean)
- \c saving=true: Save raw data (\c boolean)
- \c setbestside=never: Set the sessions's best side (\c string)
- \c keep=true: Keep changes (\c boolean)

\par Keyboard shortcuts:
- \c Left: Left speaker
- \c Right: Right speaker
- \c M, \c %Pause: Toggle mute
- \c Up: Increase intensity by 1dB
- \c Down: Decrease intensity by 1dB
- \c Shift+Up: Increase intensity by 5dB
- \c Shift+Down: Decrease intensity by 5dB
- \c Alt+Up: Increase frequency by 1kHz
- \c Alt+Down: Decrease frequency by 1kHz
- \c Alt+Shift+Up: Increase frequency by 5kHz
- \c Alt+Shift+Down: Decrease frequency by 5kHz

\version 2.2 (Jan 10, 2008)
 */


class Search : public RePro, public acoustic::Traces
{

  Q_OBJECT

public:

  Search( void );
  ~Search( void );

  virtual int main( void );

    /*! Save event data. */
  void saveEvents( const EventData &events, int count, const Options &header );


public slots:

    /*! Set intensity of stimulus to \a dB dB and emit IntensityChanged. */
  void setIntensity( int intensity );
    /*! Set duration of stimulus to \a noise ms and emit DurationChanged.*/
  void setDuration( int duration );
    /*! Set duration of pause to \a pause ms and emit PauseChanged. */
  void setPause( int pause );
    /*! Set frequency of stimulus to \a freq Hz and emit FrequencyChanged. */
  void setFrequency( int freq );
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
  virtual void dialogAccepted( void );

signals:

    /*! Intensity of stimulus changed to \a dB dB. */
  void intensityChanged( int dB );
    /*! Duration of stimulus changed to \a noise ms. */
  void durationChanged( int noise );
    /*! Duration of pause changed to \a pause ms. */
  void pauseChanged( int pause );
    /*! Frequency of stimulus changed to \a freq Hz. */
  void frequencyChanged( int freq );
    /*! Waveform of stimulus changed to \a noise. */
  void waveformChanged( int noise );


protected:

    /*! Handle keyboard events. */
  void keyPressEvent( QKeyEvent *qke );
  virtual void customEvent( QCustomEvent *qce );

  bool NewSignal;
  int SetBestSide;

  // values of search variables

  // search variables and their tuning:
  bool SearchLeft;                           // ouput channel
  QRadioButton *LeftButton;
  QRadioButton *RightButton;

  bool Mute;
  QPushButton *MuteButton;

  double Intensity;                       // (in dB)
  static const double ShortIntensityStep;
  static const double LongIntensityStep;
  static const double MaxIntensity;
  static const double MinIntensity;  

  double Duration;                         // in seconds
  static const double ShortDurationStep;
  static const double LongDurationStep;
  static const double MaxDuration;
  static const double MinDuration;

  double Pause;                         // in seconds
  double PrePause;
  static const double ShortPauseStep;
  static const double LongPauseStep;
  static const double MaxPause;
  static const double MinPause;

  double Frequency;    // in Hz
  static const double ShortFrequencyStep;
  static const double LongFrequencyStep;
  static const double MaxFrequency;
  static const double MinFrequency;
  int Waveform;
  QButtonGroup *WaveformButtons;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SEARCH_H_ */
