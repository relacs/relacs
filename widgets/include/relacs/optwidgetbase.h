/*
  optwidgetbase.h
  

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

#ifndef _RELACS_OPTWIDGETBASE_H_
#define _RELACS_OPTWIDGETBASE_H_ 1

#include <deque>
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
#include <QListWidget>
#include <QItemDelegate>
#include <QHBoxLayout>
#include <relacs/options.h>
#include <relacs/doublespinbox.h>
#include <relacs/optwidget.h>
using namespace std;

namespace relacs {


class OptWidgetBase : public QObject 
{
  Q_OBJECT
public:
  OptWidgetBase( Options::iterator param, QWidget *label,
		 Options *oo, OptWidget *ow, QMutex *mutex );
  virtual ~OptWidgetBase( void );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  void setMutex( QMutex *mutex );
  void lockMutex( void );
  bool tryLockMutex( int timeout=1 );
  void unlockMutex( void );
  void addActivation( int index, OptWidgetBase *w );
  void activateOption( bool eq );
  QWidget *valueWidget( void );
  bool editable( void ) const;
  Options::const_iterator param( void ) const;
  Options::iterator param( void );
  void setUnitLabel( QLabel *l );
 protected:
  virtual void initActivation( void );
  Options::iterator Param;
  Options *OO;
  OptWidget *OW;
  QWidget *LabelW;
  QWidget *W;
  QLabel *UnitLabel;
  QWidget *UnitBrowseW;
  QMutex *OMutex;
  bool Editable;
  bool ContUpdate;
  bool InternChanged;
  bool InternRead;
  deque< OptWidgetBase* > Widgets;
  deque< int > Index;
};


class OptWidgetText : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetText( Options::iterator param, QWidget *label, Options *oo,
		 OptWidget *ow, QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  QPushButton *browseButton( void );
  virtual void initActivation( void );
  void setUnitLabel( QLabel *l );
public slots:
  void textChanged( const QString &s );
  void browse( void );
protected:
  void doTextChanged( const QString &s );
  void doBrowse( Str filename );
  virtual void customEvent( QEvent *e );
private:
  QLineEdit *EW;
  string Value;
  QLabel *LW;
  QPushButton *BrowseButton;
};


class OptWidgetMultiText : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetMultiText( Options::iterator param, QWidget *label, Options *oo,
		      OptWidget *ow, QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  virtual void initActivation( void );
  void setUnitLabel( QLabel *l );
public slots:
  void textChanged( const QString &s );
  void insertText( const QString &text );
protected:
  void doTextChanged( const QString &s );
  void doInsertText( const QString &text );
  virtual void customEvent( QEvent *e );
private:
  QComboBox *EW;
  int CI;
  bool Inserted;
  bool Update;
  string Value;
  QLabel *LW;
};


class OptWidgetNumber : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetNumber( Options::iterator param, QWidget *label, Options *oo,
		   OptWidget *ow, QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void update( void );
  virtual void initActivation( void );
  void setUnitLabel( QLabel *l );
public slots:
  void valueChanged( double v );
protected:
  void doValueChanged( double v );
  virtual void customEvent( QEvent *e );
private:
  DoubleSpinBox *EW;
  double Value;
  QLabel *LW;
  QLCDNumber *LCDW;
};


class OptWidgetBoolean : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetBoolean( Options::iterator param, Options *oo, OptWidget *ow,
		    const string &request, QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( bool t );
protected:
  void doValueChanged( bool t );
  virtual void customEvent( QEvent *e );
private:
  QCheckBox *EW;
  bool Value;
};


class OptWidgetDate : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetDate( Options::iterator param, QWidget *label,
		 Options *oo, OptWidget *ow,
		 QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( const QDate &date );
protected:
  void doValueChanged( const QDate &date );
  virtual void customEvent( QEvent *e );
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
  OptWidgetTime( Options::iterator param, QWidget *label,
		 Options *oo, OptWidget *ow,
		 QMutex *mutex=0, QWidget *parent=0 );
  virtual void get( void );
  virtual void reset( void );
  virtual void resetDefault( void );
  virtual void initActivation( void );
public slots:
  void valueChanged( const QTime &time );
protected:
  void doValueChanged( const QTime &time );
  virtual void customEvent( QEvent *e );
private:
  QTimeEdit *TE;
  QLabel *LW;
  int Hour;
  int Minutes;
  int Seconds;
};


class OptWidgetSection : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetSection( Options::section_iterator sec,
		    Options *oo, OptWidget *ow,
		    QMutex *mutex=0, QWidget *parent=0 );
private:
  Options::section_iterator Sec;

};

class OptWidgetMultipleValues : public OptWidgetBase
{
  Q_OBJECT
public:
  OptWidgetMultipleValues( Options::iterator param, QWidget *label,
                           Options *oo, OptWidget *ow,
                           QMutex *mutex=0, QWidget *parent=0 );

  void get() override;
  void reset() override;

private slots:
  void addItem();
  void removeItem();

private:
  QWidget* Wrapper;
  QListWidget* ListWidget;
  bool Changed;
};

class NumberItemDelegate : public QItemDelegate
{
  Q_OBJECT
public:
  NumberItemDelegate(Parameter& parameter);

  QWidget *createEditor(QWidget *parent,
                        const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  Parameter& Param;
};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTWIDGETBASE_H_ */
