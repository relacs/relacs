/*
  efield/linearfield.h
  Measure the electric field manually with a single electrode in one direction

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

#ifndef _RELACS_EFIELD_LINEARFIELD_H_
#define _RELACS_EFIELD_LINEARFIELD_H_ 1

#include <QWidget>
#include <QPushButton>
#include <relacs/map.h>
#include <relacs/plot.h>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace efield {


/*!
\class LinearField
\brief [RePro] Measure the electric field manually with a single electrode in one direction
\author Jan Benda

\par Options
- \c duration=1s: Duration of measurement (\c number)

\version 1.0 (Apr 23, 2009)
*/


class LinearField : public RePro
{
  Q_OBJECT

public:

  LinearField( void );
  virtual int main( void );
  void analyze( double distance, const SampleDataF &data );
  void plot( void );
  void saveTrace( const SampleDataF &data );
  void saveAmplitude( void );


public slots:

  void measure( void );
  void finish( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  QWidget B;
  QVBoxLayout B_layout;
  OptWidget O;
  Plot P;

  Options Settings;

  Options GUIOpts;
  bool Measure;
  bool Input;

  QPushButton *MeasureButton;
  QPushButton *FinishButton;

  MapD Amplitude;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_LINEARFIELD_H_ */

