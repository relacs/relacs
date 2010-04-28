/*
  ephys/amplifiercontrol.h
  Controls an amplifier: buzzer and resistance measurement.

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

#ifndef _RELACS_EPHYS_AMPLIFIERCONTROL_H_
#define _RELACS_EPHYS_AMPLIFIERCONTROL_H_ 1

#include <qpushbutton.h>
#include <qhbox.h>
#include <relacs/misc/amplmode.h>
#include <relacs/ephys/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace ephys {


/*!
\class AmplifierControl
\brief [Control] Controls an amplifier: buzzer and resistance measurement.
\author Jan Benda
\version 1.0 (Apr 16, 2010)
*/


class AmplifierControl : public Control, public ephys::Traces
{
  Q_OBJECT

public:

  AmplifierControl( void );

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void keyReleaseEvent( QKeyEvent *e );
  virtual void initDevices( void );
  virtual void notify( void );


public slots:

    /*! Start measuring electrode resistance. */
  void startResistance( void );
    /*! Stop measuring electrode resistance. */
  void stopResistance( void );
    /*! Buzz the electrode. */
  void buzz( void );


protected:

    /*! Measure electrode resistance. */
  void measureResistance( void );


private:

  misc::AmplMode *Ampl;
  bool RMeasure;
  int DGain;
  double MaxResistance;
  double ResistanceScale;
  QHBox *AmplBox;
  QPushButton *BuzzerButton;
  QPushButton *ResistanceButton;
  QLabel *ResistanceLabel;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_AMPLIFIERCONTROL_H_ */
