/*
  ephys/amplifiercontrol.h
  Controls an amplifier: buzzer and resistance measurement.

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

#ifndef _RELACS_EPHYS_AMPLIFIERCONTROL_H_
#define _RELACS_EPHYS_AMPLIFIERCONTROL_H_ 1

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/misc/amplmode.h>
#include <relacs/ephys/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace ephys {


/*!
\class AmplifierControl
\brief [Control] Controls an amplifier: buzzer and resistance measurement.
\author Jan Benda
\version 2.0 (Apr 3, 2014)
\par Options
- \c initmode=Bridge: Initial mode of the amplifier (\c string)
- \c resistancescale=1: Scaling factor for computing R from stdev of voltage trace (\c number)
- \c maxresistance=100MOhm: Maximum resistance to be expected for scaling voltage trace (\c number)
- \c buzzpulse=500ms: Duration of buzz pulse (\c number)
- \c adjust=false: Adjust input gain for resistance measurement (\c boolean)
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
    /*! Start buzzing the electrode. */
  void startBuzz( void );
    /*! Stop buzzing the electrode. */
  void stopBuzz( void );

    /*! Activate/deactivate bridge mode of the amplifier. */
  void activateBridgeMode( bool activate=true );
    /*! Activate/deactivate current-clamp mode of the amplifier. */
  void activateCurrentClampMode( bool activate=true );
    /*! Activate/deactivate voltage-clamp mode of the amplifier. */
  void activateVoltageClampMode( bool activate=true );
    /*! Turn on manual selection of the amplifier. */
  void manualSelection( bool activate=true );


protected:

    /*! Measure electrode resistance. */
  void measureResistance( void );

  virtual void customEvent( QEvent *qce );


private:

  misc::AmplMode *Ampl;
  bool RMeasure;
  int DGain;
  bool Adjust;
  double MaxResistance;
  double ResistanceScale;
  double BuzzPulse;
  QVBoxLayout *AmplBox;
  QHBoxLayout *BuzzBox;
  QPushButton *BuzzerButton;
  QPushButton *ResistanceButton;
  QLabel *ResistanceLabel;
  QGroupBox *ModeBox;
  QRadioButton *BridgeButton;
  QRadioButton *CCButton;
  QRadioButton *VCButton;
  QRadioButton *ManualButton;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_AMPLIFIERCONTROL_H_ */
