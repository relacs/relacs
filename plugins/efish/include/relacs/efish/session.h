/*
  efish/session.h
  Session for in vivo recordings in weakly electric fish.

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

#ifndef _RELACS_EFISH_SESSION_H_
#define _RELACS_EFISH_SESSION_H_ 1

#include <qgrid.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace efish {


/*!
\class Session
\brief [Control] Session for in vivo recordings in weakly electric fish.
\author Jan Benda
\version 1.0 (Nov 27, 2009)
*/


class Session
  : public Control,
    public ephys::Traces,
    public efield::Traces
{
  Q_OBJECT

public:

  Session( void );
  ~Session( void );

  virtual void initialize( void );
  virtual void config( void );

  virtual void sessionStarted( void );
  virtual void sessionStopped( bool saved );
  void saveEOD( void );

  void plot( void );
  virtual void main( void );


public slots:

  void customEvent( QCustomEvent *qce );


protected:

    /*! This function is called when an option of the Session is set. */
  virtual void notify( void );


private:

  double PlotWindow;

  double EODRate;
  MapD EODRates;
  double EODAmplitude;
  MapD EODAmplitudes;
  string EODUnit;
  int EODUpdate;
  long EODOffset;

  MultiPlot EODPlot;
  QGrid *Numbers;
  QLCDNumber *EODRateLCD;
  QLCDNumber *FiringRateLCD[MaxSpikeTraces];
  QLCDNumber *PValueLCD[MaxSpikeTraces];
  QPushButton *SessionButton;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_SESSION_H_ */
