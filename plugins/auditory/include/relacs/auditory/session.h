/*
  auditory/session.h
  Session for recordings from auditory neurons

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

#ifndef _RELACS_AUDITORY_SESSION_H_
#define _RELACS_AUDITORY_SESSION_H_ 1

#include <QPushButton>
#include <QWidget>
#include <relacs/optwidget.h>
#include <relacs/multiplot.h>
#include <relacs/map.h>
#include <relacs/temperature.h>
//#include <relacs/misc/amplmode.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace auditory {


/*! 
\class Session
\brief [Control] Control recordings from auditory neurons
\author Jan Benda
\todo Temperature and Resistance need also to be displayed in the widget.
\version 1.4 (Mar 1, 2009)
*/


class Session : public Control, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  Session( void );
  ~Session( void );

  virtual void initialize( void );
  virtual void config( void );
  virtual void initDevices( void );
  virtual void startSession( void );
  virtual void stopSession( bool saved );
  virtual void keyPressEvent( QKeyEvent *e );
  virtual void keyReleaseEvent( QKeyEvent *e );

    /*! Notify about changes in the meta data. */
  virtual void notifyMetaData( const string &section );

    /*! Return the most recently measured threshold curve
        of side \a side.
	If \a side equals 2, then the threshold curve of the cell's best side
	is returned. */
  MapD threshCurve( int side=2 ) const;
    /*! Pass a measured threshold curve \a thresh of side \a side to the session. */
  void addThreshCurve( const MapD &thresh, int side );
    /*! Return the most recently measured f-I curve
        of side \a side.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned. */
  MapD fICurve( int side=2 ) const;
    /*! Pass a measured f-I curve \a ficurve of side \a side to the session. */
  void addFICurve( const MapD &ficurve, int side );
    /*! Return the most recently measured onset f-I curve
        of side \a side.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned. */
  MapD onFICurve( int side=2 ) const;
    /*! Pass a measured onset f-I curve \a onficurve of side \a side to the session. */
  void addOnFICurve( const MapD &onficurve, int side );
    /*! Return the most recently measured steady-state f-I curve
        of side \a side.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned. */
  MapD ssFICurve( int side=2 ) const;
    /*! Pass a measured steady state f-I curve \a ssficurve of side \a side to the session. */
  void addSSFICurve( const MapD &ssficurve, int side );

  /*! Determine the cell's best side based on the left / right and
      left noise / right noise settings. */
  void updateBestSide( void );


public slots:

    /*! Start measuring electrode resistance. */
  void startResistance( void );
    /*! Stop measuring electrode resistance. */
  void stopResistance( void );
    /*! Buzz the electrode. */
  void buzz( void );


protected:

  void main( void );


private:

    /*! Update the plots. */
  void plot( void );

    /*! Measure electrode resistance. */
  void measureResistance( void );

    /*! The last threshold curve of the previous session for each side. */
  MapD OldThreshCurve[2];
    /*! All the threshold curves of the current session for each side. */
  vector< MapD > ThreshCurve[2];
    /*! The last f-I curve of the previous session for each side. */
  MapD OldFICurve[2];
    /*! All the f-I curves of the current session for each side. */
  vector< MapD > FICurve[2];
    /*! All the onset f-I curves of the current session for each side. */
  vector< MapD > OnFICurve[2];
    /*! All the steady-state f-I curves of the current session for each side. */
  vector< MapD > SSFICurve[2];

  MultiPlot P;
  OptWidget *ASW;
  static const int MetaDataReadOnly = 1;
  static const int MetaDataDisplay = 2;
  static const int MetaDataReset = 4;
  static const int MetaDataSave = 8;

  Temperature *Temp;
  //  misc::AmplMode *Ampl;
  bool RMeasure;
  int DGain;
  double MaxResistance;
  double ResistanceScale;
  QWidget *AmplBox;
  QPushButton *ResistanceButton;
  QPushButton *BuzzerButton;

  QPushButton *SessionButton;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SESSION_H_ */
