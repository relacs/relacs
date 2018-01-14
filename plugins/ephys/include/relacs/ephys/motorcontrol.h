/*
  ephys/motorcontrol.h
  Control widgets for motorized manipulators.

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

#ifndef _RELACS_EPHYS_MOTORCONTROL_H_
#define _RELACS_EPHYS_MOTORCONTROL_H_ 1

#include <vector>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTimer>
#include <relacs/optwidget.h>
#include <relacs/configclass.h>
#include <relacs/control.h>
using namespace relacs;

namespace ephys {


class MiMaPu;

/*! 
\class MotorControl
\brief [Control] Control widgets for motorized manipulators.
\author Jan Benda
\bug reimplement as thread and avoid use of QTimer!
\bug spikes( int trace ): instead of sessionOpts we should access the spikedtector directly!
\bug check locking!

\par Manipulator Control
The z-axis of the manipulator is moved in steps. 
Each step has a certain amplitude.
To make steps in both directions similar, the amplitude of each step
can be different for upwards and downwards movements.
This can be controlled by the asymmetry factor.

Parameter:
- speed: The speed is the number of steps executed at once.
- amplitude: The amplitude of a single step (1-80)
- asymmetry: The asymmetry between upwards and downwards amplitudes.
                1.0: both amplitudes are equal.
- interval: The minimum interval between successive movements.
.

Buttons:
- set home: Set the current position of the z-axis as the home position.
- go home: Move the z-axis back to zero, i.e. back to its home position.
- go: Advance the manipulator every \a interval ms by \a speed steps
         until a spiking neuron is encountered.
- stop: Stop automatic advance.
.

Key-Shortcuts for Manipulator 1:
- \c Up: Move manipulator 1 up.
- \c Down: Move manipulator 1 down.
- \c Shift+ \c Up: Increase speed of manipulator 1.
- \c Shift+ \c Down: Decrease speed of manipulator 1.
- \c Alt+ Up: Increase amplitude of manipulator 1.
- \c Alt+ \c Down: Decrease amplitude of manipulator 1.
- \c Ctrl+ \c Up: Increase interval of manipulator 1.
- \c Ctrl+ \c Down: Decrease interval of manipulator 1.
- \c G: Toggel go/stop for manipulator 1.
.

Key-Shortcuts for Manipulator 1:
- \c Left: Move manipulator 2 up.
- \c Right: Move manipulator 2 down.
- \c Shift+ \c Left: Increase speed of manipulator 2.
- \c Shift+ \c Right: Decrease speed of manipulator 2.
- \c Alt+ Left: Increase amplitude of manipulator 2.
- \c Alt+ \c Right: Decrease amplitude of manipulator 2.
- \c Ctrl+ \c Left: Increase interval of manipulator 2.
- \c Ctrl+ \c Right: Decrease interval of manipulator 2.
- \c H: Toggel go/stop for manipulator 2.

\par Options
- \c minrate=20Hz: Minimum Rate (\c number)
- \c minsize=20mV: Minimum Size (\c number)

\version 1.0 (Jul 1 2004)
*/


class MotorControl : public Control
{
  Q_OBJECT

public:

  MotorControl( void );
  ~MotorControl( void );

  virtual void preConfigure( void );
  virtual void initDevices( void );
  virtual void notify( void );
  virtual void keyPressEvent( QKeyEvent *e );

  int spikes( int trace );
  QVBoxLayout *mainLayout( void );

  friend class MiMaPu;


private:

  vector< int > SpikeEvents;

  vector< MiMaPu* > MM;

  double MinRate;
  double MinSize;

  QVBoxLayout *MainLayout;

  QPixmap BadQuality;
  QPixmap PotentialQuality;
  QPixmap OkQuality;
  QPixmap GoodQuality;
  QPixmap *QualityPixs[4];

  QPixmap BadArrow;
  QPixmap BadTrend;
  QPixmap OkTrend;
  QPixmap GoodTrend;
  QPixmap GoodArrow;
  QPixmap *TrendPixs[5];

};

/*! 
\class MiMaPu
\brief [lib] A single manipulator for %MotorControl.
\author Jan Benda
*/

class MiMaPu : public QObject, public ConfigClass
{
  Q_OBJECT

public:

  MiMaPu( Manipulator *m, int trace, const string &title, 
	  const Options &detect, MotorControl *parent );
  void keyUp( int mode );
  void keyDown( int mode );
  virtual void notify( void );

public slots:

  void setHome( void );
  void moveToHome( void );
  void find( void );
  void stop( void );


private:

  MotorControl *MC;
  int Trace;
  Manipulator *M;
  double Interval;
  int Steps;
  int Back;
  bool Dir;
  int Amplitude;
  double AmplAsymm;
  OptWidget *OW;
  QLabel *QualityIndicator;
  QLabel *TrendIndicator;
  QLabel *SpikeSize;
  QLabel *SpikeRate;
  const Options &Detect;
  bool Find;
  bool StartFind;
  QPushButton *FindButton;
  QTimer *T;
  QTime LastStep;


private slots:

  void update( void );

};


}; /* namespace ephys */

#endif /* ! _RELACS_COMMON_MOTORCONTROL_H_ */
