/*
  patchclamp/setdc.h
  Interactively set the DC current.

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

#ifndef _RELACS_PATCHCLAMP_SETDC_H_
#define _RELACS_PATCHCLAMP_SETDC_H_ 1

#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <relacs/ephys/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace patchclamp {


/*!
\class SetDC
\brief [RePro] Interactively set the DC current.
\author Jan Benda
\version 1.3 (Nov 25, 2010)
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
- \c duration=500ms: Duration for analysis (\c number)
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

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );
  QDoubleSpinBox *EW;
  QLabel *UnitLabel;
  QLabel *VoltageLabel;
  QLabel *RateLabel;
  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ResetButton;
  QPushButton *ZeroButton;
  bool SetValue;
  bool Finished;
  int OutCurrent;
  string IUnit;
  double DCAmplitude;
  double OrgDCAmplitude;


protected slots:
  
  void setValue( double value );
  void setValue( void );
  void keepValue( void );
  void resetDC( void );
  void zeroDC( void );

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SETDC_H_ */
