/*
  patchclamp/setdc.h
  Interactively set the DC current.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
\version 1.0 (Feb 09, 2010)
*/


  class SetDC : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  SetDC( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );
  QDoubleSpinBox *EW;
  QLabel *UnitLabel;
  QPushButton *OKButton;
  QPushButton *CancelButton;
  bool SetValue;
  bool Finished;
  int OutCurrent;
  string IUnit;
  double DCAmplitude;


protected slots:
  
  void setValue( double value );
  void setValue( void );
  void keepValue( void );

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SETDC_H_ */
