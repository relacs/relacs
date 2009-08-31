/*
  optwidgetbase.h
  

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

#ifndef _RELACS_OPTWIDGETBASE_H_
#define _RELACS_OPTWIDGETBASE_H_ 1

#include <vector>
#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QLabel>
#include <QLCDNumber>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <relacs/options.h>
#include "optwidget.h"
using namespace std;

namespace relacs {


class OptWidgetBase : public QObject 
{
  Q_OBJECT
public:
  OptWidgetBase( Options::iterator op, QWidget *label,
		 Options *oo, OptWidget *ow, QMutex *mutex );
  virtual ~OptWidgetBase( void );
  virtual void get( void ) {};
  virtual void reset( void ) {};
  virtual void resetDefault( void ) {};
  virtual void update( void ) { reset(); };
  void setMutex( QMutex *mutex ) { OMutex = mutex; };
  void setUnitBrowseW( QWidget *w ) { UnitBrowseW = w; };
  void addActivation( OptWidgetBase *w );
  virtual void initActivation( void );
  void activateOption( bool eq );
  Options::iterator OP;
  Options *OO;
  OptWidget *OW;
  QWidget *LabelW;
  QWidget *W;
  QWidget *UnitBrowseW;
  QMutex *OMutex;
  bool Editable;
  bool ContUpdate;
  bool InternChanged;
  vector< OptWidgetBase* > Widgets;
};


class OptWidgetText : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetText( Options::iterator op, QWidget *label, Options *oo,
		 OptWidget *ow, QWidget *parent, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  void setUnitLabel( QLabel *l );
  QPushButton *browseButton( void );
  virtual void initActivation( void );
public slots:
  void textChanged( const QString &s );
  void browse( void );
private:
  QLineEdit *EW;
  string Value;
  QLabel *LW;
  QLabel *UnitLabel;
  QPushButton *BrowseButton;
};


class OptWidgetMultiText : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetMultiText( Options::iterator op, QWidget *label, Options *oo,
		      OptWidget *ow, QWidget *parent, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  void setUnitLabel( QLabel *l );
  virtual void initActivation( void );
public slots:
  void textChanged( const QString &s );
  void insertText( const QString &text );
private:
  QComboBox *EW;
  int CI;
  bool Inserted;
  bool Update;
  string Value;
  QLabel *LW;
  QLabel *UnitLabel;
};


class OptWidgetNumber : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetNumber( Options::iterator op, QWidget *label, Options *oo,
		   OptWidget *ow, QWidget *parent, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  void setUnitLabel( QLabel *l );
  virtual void initActivation( void );
public slots:
  void valueChanged( double v );
private:
  QDoubleSpinBox *EW;
  double Value;
  QLabel *LW;
  QLCDNumber *LCDW;
  QLabel *UnitLabel;
};


class OptWidgetBoolean : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetBoolean( Options::iterator op, Options *oo, OptWidget *ow,
		    QWidget *parent, const string &request, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( bool t );
  void dontToggle( bool t );
private:
  QCheckBox *EW;
  bool Value;
};


class OptWidgetDate : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetDate( Options::iterator op, Options *oo, OptWidget *ow,
		 QWidget *parent, const string &request, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( const QDate &date );
private:
  QDateEdit *DE;
  QLabel *LW;
  int Year;
  int Month;
  int Day;
};


class OptWidgetTime : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetTime( Options::iterator op, Options *oo, OptWidget *ow,
		 QWidget *parent, const string &request, QMutex *mutex=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( const QTime &time );
private:
  QTimeEdit *TE;
  QLabel *LW;
  int Hour;
  int Minutes;
  int Seconds;
};


class OptWidgetLabel : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetLabel( Options::iterator op, Options *oo, OptWidget *ow,
		  QWidget *parent, QMutex *mutex=0 );
};


class OptWidgetSeparator : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetSeparator( Options::iterator op, Options *oo, OptWidget *ow,
		      QWidget *parent, QMutex *mutex=0 );
};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTWIDGETBASE_H_ */
