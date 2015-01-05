/*
  patchclamp/setdc.h
  Interactively set the DC current.

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

#ifndef _RELACS_PATCHCLAMP_SETDC_H_
#define _RELACS_PATCHCLAMP_SETDC_H_ 1

#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace patchclamp {


/*!
\class SetDC
\brief [RePro] Interactively set the DC current.
\author Jan Benda
\version 1.6 (Sep 13, 2014)
\par Screenshot
\image html setdc.png

\par Options
- \c outcurrent=Current-1: Output trace (\c string)
- \c dcamplitudesel=to absolute value: Set DC amplitude (\c string)
- \c dcamplitude=0nA: DC amplitude (\c number)
- \c dcamplitudefrac=90%: Fraction of threshold (\c number)
- \c dcamplitudedecr=0.1nA: Decrement below threshold (\c number)
- \c interactive=false: Set dc amplitude interactively (\c boolean)
- \c dcamplitudestep=0.001nA: Stepsize for entering dc (\c number)
- \c showstep=true: Show stepsize widget (\c boolean)
- \c duration=2seconds: Duration for analysis (\c number)
- \c update=0.5seconds: Update time for analysis (\c number)
- \c showstdev=true: Print standard deviation of voltage (\c boolean)

\par Key shortcuts
- \c ALT+ArrowUp: Increase stepsize for setting the DC current
- \c ALT+ArrowUp: Decrease stepsize for setting the DC current
*/


class SetDC : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  SetDC( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


protected:

  void analyze( double duration );
  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );
  QDoubleSpinBox *EW;
  QComboBox *CW;
  QLabel *DCStepLabel;
  QLabel *CurrentUnitLabel1;
  QLabel *CurrentUnitLabel2;
  QLabel *VoltageLabel;
  QLabel *VoltageUnitLabel;
  QLabel *RateLabel;
  MultiPlot P;
  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ResetButton;
  QPushButton *ZeroButton;
  bool ShowStdev;
  bool SetValue;
  bool Finished;
  string IUnit;
  double DCAmplitude;
  double OrgDCAmplitude;
  double DCStep;
  int DCStepIndex;
  static const int NDCSteps = 10;
  static const double DCSteps[NDCSteps];


protected slots:
  
  void setValue( double value );
  void setValue( void );
  void keepValue( void );
  void resetDC( void );
  void zeroDC( void );
  void setStep( int index );

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SETDC_H_ */
