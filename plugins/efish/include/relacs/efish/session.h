/*
  efish/session.h
  Session for recording EODs of weakly electric fish.

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

#ifndef _RELACS_EFISH_SESSION_H_
#define _RELACS_EFISH_SESSION_H_ 1

#include <QGridLayout>
#include <QPushButton>
#include <QLCDNumber>
#include <relacs/multiplot.h>
#include <relacs/temperature.h>
#include <relacs/base/traces.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
#include <relacs/control.h>
using namespace relacs;

namespace efish {


/*!
\class Session
\brief [Control] %Session for recording EODs of weakly electric fish.
\author Jan Benda
\version 1.1 (Jan 19, 2013)
\par Options
- \c ephys=true: Electrophysiological recording (\c boolean)
\par Plots
- The amplitude (red) and
- the frequency (green)
of the global EOD measurement.
*/


class Session
  : public Control,
    public base::Traces,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  Session( void );
  ~Session( void );

  virtual void preConfig( void );
  virtual void config( void );
  virtual void initDevices( void );
  virtual void clearDevices( void );

  virtual void sessionStarted( void );
  virtual void sessionStopped( bool saved );
  void saveEOD( void );
  void saveTemperature( void );

  void plot( void );
  virtual void main( void );


public slots:

  void customEvent( QEvent *qce );


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
  double TimeOffset;

  Temperature *TempDev;
  double WaterTemp;
  MapD WaterTemps;
  long TemperatureOffset;

  MultiPlot EODPlot;
  QGridLayout *Numbers;
  QLCDNumber *EODRateLCD;
  QLCDNumber *FiringRateLCD[MaxTraces];
  QLCDNumber *PValueLCD[MaxTraces];
  QPushButton *SessionButton;

  bool EPhys;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_SESSION_H_ */
