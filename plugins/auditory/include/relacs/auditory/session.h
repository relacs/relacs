/*
  auditory/session.h
  Session for recordings from auditory neurons

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

#ifndef _RELACS_AUDITORY_SESSION_H_
#define _RELACS_AUDITORY_SESSION_H_ 1

#include <QPushButton>
#include <QWidget>
#include <relacs/optwidget.h>
#include <relacs/multiplot.h>
#include <relacs/map.h>
#include <relacs/temperature.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace auditory {


/*! 
\class Session
\brief [Control] Control recordings from auditory neurons
\author Jan Benda
\todo Temperature needs also to be displayed in the widget.
\version 1.5 (Feb 2, 2010)
*/


class Session : public Control, public ephys::Traces, public acoustic::Traces
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

    /*! Notify about changes in the meta data. */
  virtual void notifyMetaData( void );
  virtual void notifyStimulusData( void );

    /*! Return the most recently measured threshold curve
        of side \a side.
	If \a side equals 2, then the threshold curve of the cell's best side
	is returned. */
  MapD threshCurve( int side=2 ) const;
    /*! Pass a measured threshold curve \a thresh of side \a side to the session. */
  void addThreshCurve( const MapD &thresh, int side );
    /*! Return the most recently measured f-I curve
        of side \a side and carrier frequency \a carrierfreq.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned.
        If \a carrierfreq equals 0.0, then the f-I curve at the
	best frequency is returned.
        Setting \a lockit to \c false does not lock the Session's mutex
	while accessing the f-I curve data (for internal usage). */
  MapD fICurve( int side=2, double carrierfreq=0.0, bool lockit=true ) const;
    /*! Return the \a index-th most recently measured f-I curve and its 
        carrier frequency \a carrierfreq of side \a side.
	If \a side equals 2, then an f-I curve of the cell's best side
	is returned.
	\a index=0 is the most recently measured f-I curve.
        If \a index requests an f-I curve that does not exist,
	an empty MapD is returned and \a carrierfreq is set to 0. */
  MapD fICurve( int index, int side, double &carrierfreq ) const;
    /*! Pass a measured f-I curve \a ficurve of side \a side 
        with carrier frequency \a carrierfreq to the session. */
  void addFICurve( const MapD &ficurve, int side, double carrierfreq );
    /*! Return the most recently measured onset f-I curve
        of side \a side and carrier frequency \a carrierfreq.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned.
        If \a carrierfreq equals 0.0, then the f-I curve at the
	best frequency is returned. */
  MapD onFICurve( int side=2, double carrierfreq=0.0 ) const;
    /*! Return the \a index-th most recently measured onset f-I curve and its 
        carrier frequency \a carrierfreq of side \a side.
	If \a side equals 2, then an f-I curve of the cell's best side
	is returned.
	\a index=0 is the most recently measured onset f-I curve.
        If \a index requests an f-I curve that does not exist,
	an empty MapD is returned and \a carrierfreq is set to 0. */
  MapD onFICurve( int index, int side, double &carrierfreq ) const;
    /*! Pass a measured onset f-I curve \a onficurve of side \a side 
        with carrier frequency \a carrierfreq to the session. */
  void addOnFICurve( const MapD &onficurve, int side, double carrierfreq );
    /*! Return the most recently measured steady-state f-I curve
        of side \a side and carrier frequency \a carrierfreq.
	If \a side equals 2, then the f-I curve of the cell's best side
	is returned.
        If \a carrierfreq equals 0.0, then the f-I curve at the
	best frequency is returned. */
  MapD ssFICurve( int side=2, double carrierfreq=0.0 ) const;
    /*! Return the \a index-th most recently measured steady-state f-I curve and its 
        carrier frequency \a carrierfreq of side \a side.
	If \a side equals 2, then an f-I curve of the cell's best side
	is returned.
	\a index=0 is the most recently steady-state measured f-I curve.
        If \a index requests an f-I curve that does not exist,
	an empty MapD is returned and \a carrierfreq is set to 0. */
  MapD ssFICurve( int index, int side, double &carrierfreq ) const;
    /*! Pass a measured steady state f-I curve \a ssficurve of side \a side 
        with carrier frequency \a carrierfreq to the session. */
  void addSSFICurve( const MapD &ssficurve, int side, double carrierfreq );

  /*! Determine the cell's best side based on the left / right and
      left noise / right noise settings. */
  void updateBestSide( void );


protected:

  void main( void );


private:

    /*! Update the plots. */
  void plot( void );

    /*! The last threshold curve of the previous session for each side. */
  MapD OldThreshCurve[2];
    /*! All the threshold curves of the current session for each side. */
  vector< MapD > ThreshCurve[2];
    /*! The last f-I curve of the previous session for each side at best frequency. */
  MapD OldFICurve[2];
    /*! All the f-I curves of the current session for each side. */
  vector< MapD > FICurve[2];
    /*! The corresponding carrier frequencies for FICurve. */
  vector< double > FICurveCarrier[2];
    /*! All the onset f-I curves of the current session for each side. */
  vector< MapD > OnFICurve[2];
    /*! The corresponding carrier frequencies for OnFICurve. */
  vector< double > OnFICurveCarrier[2];
    /*! All the steady-state f-I curves of the current session for each side. */
  vector< MapD > SSFICurve[2];
    /*! The corresponding carrier frequencies for SSFICurve. */
  vector< double > SSFICurveCarrier[2];

  MultiPlot P;
  OptWidget *ASW;
  static const int MetaDataReadOnly = 1;
  static const int MetaDataDisplay = 2;
  static const int MetaDataReset = 4;
  static const int MetaDataSave = 8;
  OptWidget *SW;

  Temperature *Temp;

  QPushButton *SessionButton;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SESSION_H_ */
