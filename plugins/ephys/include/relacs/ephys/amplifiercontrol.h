/*
  ephys/amplifiercontrol.h
  Controls an amplifier: buzzer and resistance measurement.

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

#ifndef _RELACS_EPHYS_AMPLIFIERCONTROL_H_
#define _RELACS_EPHYS_AMPLIFIERCONTROL_H_ 1

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/digitalio.h>
#include <relacs/doublespinbox.h>
#include <relacs/misc/amplmode.h>
#include <relacs/ephys/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace ephys {


/*!
\class AmplifierControl
\brief [Control] Controls an amplifier: buzzer and resistance measurement.
\author Jan Benda
\version 3.2 (Oct 18, 2018)

\par Options
- \c initmode=Bridge: Initial mode of the amplifier (\c string)
- \c resistancecurrent=1nA: The average current of the amplifier used for measuring electrode resistance (\c number)
- \c adjust=false: Adjust input gain for resistance measurement (\c boolean)
- \c maxresistance=100MOhm: Maximum resistance to be expected for scaling voltage trace (\c number)
- \c buzzpulse=500ms: Duration of buzz pulse (\c number)
- \c showswitchmessage=true: Show message for manually switching the amplifier mode (\c boolean)
- \c showbridge=true: Make bridge mode for amplifier selectable (\c boolean)
- \c showcc=false: Make current clamp mode for amplifier selectable (\c boolean)
- \c showdc=false: Make dynamic clamp mode for amplifier selectable (\c boolean)
- \c showvc=false: Make voltage clamp mode for amplifier selectable (\c boolean)
- \c showmanual=false: Make manual mode for amplifier selectable (\c boolean)
- \c syncpulse=10us: Duration of SEC current injection (\c number)
- \c syncmode=0samples: Interval is average over (\c integer);
- \c vcgain=100: VC gain (\c number)
- \c vctau=1ms: VC time constant (\c number)

\par Key shortcuts
- \c Z: Buzz
- \c .: Buzz
- \c O: Measure resistance ("O" is the shortcut of "Ohm"!)
- \c ALT-B: Switch amplifier to bridge mode
- \c ALT-C: Switch amplifier to current-clamp mode
- \c ALT-Y: Switch amplifier to dynamic-clamp mode
- \c ALT-V: Switch amplifier to voltage-clamp mode
- \c ALT-A: Switch to manual selection of amplifier mode

\par Use a presenter to control RELACS
You can map key presses of a presenter to keys RELACS understands as follows:
- Look in \c /lib/udev/hwdb.d/60-keyboard.hwdb for your presenter/keyboard
- Copy the lines to a file \c 70-keyboard-relacs.hwdb in \c /etc/udev/hwdb.d/
- Edit these lines according to your needs.
  Each line maps a scancode (as hex-value) to a keycode (lower case).
  For example, the four buttons of a Logitech R400 can be remapped as follows:
\code
# Logitech Presenter R400
keyboard:usb:v046DpC52Dd*dc*dsc*dp*ic*isc*ip*in00*
 KEYBOARD_KEY_7004B=space
 KEYBOARD_KEY_7004E=f1
 KEYBOARD_KEY_70029=dot
 KEYBOARD_KEY_7003E=dot
 KEYBOARD_KEY_70037=v
\endcode
 With this, the "arrow left" terminates the running repro gracefully by sending "space".
 "arrow right" starts the macro bound to the "F1" key, the "start presentation" button
 activates the buzzer, and the "blank screen" button centers the traces in the plot.
- inform your system about the new key bindings (as root):
\code
# udevadm hwdb --update
\endcode
- and then you need to reboot (!) so that these key mappings take into effect
- to find out what scancodes your device emits use (as root)
\code
# evtest
\endcode
- possible keycodes are listed at http://hal.freedesktop.org/quirk/quirk-keymap-list.txt
  or in the header file /usr/include/linux/input.h (the long section with the \c #define KEY_1 etc.
*/

class AmplifierControl : public Control, public ephys::Traces
{
  Q_OBJECT

public:

  AmplifierControl( void );

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void keyReleaseEvent( QKeyEvent *e );
  virtual void initDevices( void );
  virtual void clearDevices( void );
  virtual void notify( void );
    /*! If idle() sets manual mode selection for the amplifier. */
  virtual void modeChanged( void );


public slots:

    /*! Mute audio monitor and call doResistance() a little bit delayed. */
  void startResistance( void );
    /*! Start measuring electrode resistance. */
  void doResistance( void );
    /*! Stop measuring electrode resistance. */
  void stopResistance( void );
    /*! Mute audio monitor and call doBuzz() a little bit delayed. */
  void startBuzz( void );
    /*! Start buzzing the electrode. */
  void doBuzz( void );
    /*! Stop buzzing the electrode. */
  void stopBuzz( void );

    /*! Activate/deactivate bridge mode of the amplifier. */
  void activateBridgeMode( bool activate=true );
    /*! Activate/deactivate current-clamp mode of the amplifier. */
  void activateCurrentClampMode( bool activate=true );
    /*! Activate/deactivate current-clamp mode 
        with external synchronization of the amplifier. */
  void activateDynamicClampMode( bool activate=true );
    /*! Activate/deactivate voltage-clamp mode of the amplifier. */
  void activateVoltageClampMode( bool activate=true );
    /*! Turn on manual selection of the amplifier. */
  void manualSelection( bool activate=true );

    /*! Set duration of the synchronization pulse
        to \a durationus microseconds. */
  void setSyncPulse( double durationus );
    /*! Set mode of the synchronization pulse
        to \a mode. */
  void setSyncMode( int mode );

    /*! Set voltage clamp gain to \a vcgain. */
  void setVCGain( double vcgain );
    /*! Set voltage clamp time constant to \a vctaums milliseconds. */
  void setVCTau( double vctaums );


protected:

    /*! Resets stimulus metadata regarding amplifier synchronization. */
  void clearSyncPulse( void );

    /*! Measure electrode resistance. */
  void measureResistance( void );

  virtual void customEvent( QEvent *qce );


private:

  misc::AmplMode *Ampl;
  bool RMeasure;
  int DGain;
  bool Adjust;
  double MaxResistance;
  double ResistanceCurrent;
  double BuzzPulse;
  bool ShowSwitchMessage;
  QVBoxLayout *AmplBox;
  QHBoxLayout *BuzzBox;
  QPushButton *BuzzerButton;
  QPushButton *ResistanceButton;
  QLabel *ResistanceLabel;
  QGroupBox *ModeBox;
  QRadioButton *BridgeButton;
  QRadioButton *CCButton;
  QRadioButton *DCButton;
  QRadioButton *VCButton;
  QRadioButton *ManualButton;
  QWidget *DCPulseBox;
  DoubleSpinBox *SyncPulseSpinBox;
  QSpinBox *SyncModeSpinBox;
  bool SyncPulseEnabled;
  double SyncPulseDuration;
  int SyncMode;
  QWidget *VCBox;
  DoubleSpinBox *VCGainSpinBox;
  DoubleSpinBox *VCTauSpinBox;
  double VCGain;
  double VCTau;
  bool DoBuzz;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_AMPLIFIERCONTROL_H_ */
