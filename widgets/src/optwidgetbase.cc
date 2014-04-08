/*****************************************************************************
 *
 * optwidgetbase.cc
 * 
 *
 * RELACS
 * Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RELACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <cmath>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QFileDialog>
#include <relacs/optwidgetbase.h>


namespace relacs {


OptWidgetBase::OptWidgetBase( Options::iterator param, QWidget *label,
			      Options *oo, OptWidget *ow, QMutex *mutex )
  : QObject(),
    Param( param ),
    OO( oo ),
    OW( ow ),
    LabelW( label ),
    W( 0 ),
    UnitLabel( 0 ),
    UnitBrowseW( 0 ),
    OMutex( mutex ),
    Editable( true ),
    ContUpdate( ow->continuousUpdate() ),
    InternChanged( false ),
    InternRead( false )
{
  if ( Param == OO->end() || OW->readOnlyMask() < 0 ||
       ( OW->readOnlyMask() > 0 && ( Param->flags() & OW->readOnlyMask() ) ) )
    Editable = false;
  else
    Param->delFlags( OW->changedFlag() );
  ow->addWidget( this );
}


OptWidgetBase::~OptWidgetBase( void )
{
}

void OptWidgetBase::get( void )
{
}


void OptWidgetBase::reset( void )
{
}


void OptWidgetBase::resetDefault( void )
{
}


void OptWidgetBase::update( void )
{
}


void OptWidgetBase::setMutex( QMutex *mutex )
{
  OMutex = mutex;
}


void OptWidgetBase::lockMutex( void )
{
  if ( OMutex != 0 )
    OMutex->lock();
}


bool OptWidgetBase::tryLockMutex( int timeout )
{
  if ( OMutex != 0 )
    return OMutex->tryLock( timeout );
  else
    return true;
}


void OptWidgetBase::unlockMutex( void )
{
  if ( OMutex != 0 )
    OMutex->unlock();
}


QWidget *OptWidgetBase::valueWidget( void )
{
  return W;
}


bool OptWidgetBase::editable( void ) const
{
  return Editable;
}


Options::const_iterator OptWidgetBase::param( void ) const
{
  return Param;
}


Options::iterator OptWidgetBase::param( void )
{
  return Param;
}


void OptWidgetBase::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = 0;
  }
}


void OptWidgetBase::initActivation( void )
{
}


void OptWidgetBase::addActivation( OptWidgetBase *w )
{
  Widgets.push_back( w );
  initActivation();
}


void OptWidgetBase::activateOption( bool eq )
{
  bool ac = Param != OO->end() ? ( Param->activation() ? eq : !eq ) : true;

  if ( LabelW != 0 )
    LabelW->setEnabled( ac );
  if ( W != 0 )
    W->setEnabled( ac );
  if ( UnitLabel != 0 )
    UnitLabel->setEnabled( ac );
  if ( UnitBrowseW != 0 )
    UnitBrowseW->setEnabled( ac );
}


OptWidgetText::OptWidgetText( Options::iterator param, QWidget *label,
			      Options *oo, OptWidget *ow,
			      QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, label, oo, ow, mutex ),
    EW( 0 ),
    Value( "" ),
    LW( 0 ),
    BrowseButton( 0 )
{
  if ( (Param->style() & OptWidget::SelectText) > 0 )
    Editable = false;
  if ( Editable ) {
    W = EW = new QLineEdit( Param->text( "%s" ).c_str(), parent );
    OptWidget::setValueStyle( W, Param->style(), OptWidget::Text );
    Value = EW->text().toStdString();
    connect( EW, SIGNAL( textChanged( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
    if ( Param->style() & OptWidget::Browse ) {
      BrowseButton = new QPushButton( "Browse...", parent );
      UnitBrowseW = BrowseButton;
      connect( BrowseButton, SIGNAL( clicked( void ) ),
	       this, SLOT( browse( void ) ) );
    }
  }
  else {
    LW = new QLabel( Param->text( "%s" ).c_str(), parent );
    OptWidget::setValueStyle( LW, Param->style(), OptWidget::Window );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetText::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setText( EW->text().toStdString() );
    if ( Param->text( 0, "%s" ) != Value )
      Param->addFlags( OW->changedFlag() );
    Value = EW->text().toStdString();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetText::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    EW->setText( Param->text( 0, "%s" ).c_str() );
  }
  else {
    LW->setText( Param->text( 0, "%s" ).c_str() );
  }
  InternChanged = false;
}


void OptWidgetText::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setText( Param->defaultText( "%s" ).c_str() );
    InternChanged = false;
  }
}


void OptWidgetText::update( void )
{
  if ( UnitLabel != 0 ) {
    InternChanged = true;
    UnitLabel->setText( Param->outUnit().htmlUnit().c_str() );
    InternChanged = false;
  }
}


void OptWidgetText::textChanged( const QString &s )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable ) {
    if ( InternChanged ) {
      Value = EW->text().toStdString();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setText( Value );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doTextChanged( s );
  }
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( s.toStdString() ) );
  }
}


class OptWidgetTextEvent : public QEvent
{
public:
  OptWidgetTextEvent( int type )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Text( "" ), FileName( "" ) {};
  OptWidgetTextEvent( int type, const QString &s )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Text( s ), FileName( "" ) {};
  OptWidgetTextEvent( int type, const Str &filename )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Text( "" ), FileName( filename ) {};
  QString text( void ) const { return Text; };
  Str fileName( void ) const { return FileName; };
private:
  QString Text;
  Str FileName;
};


void OptWidgetText::doTextChanged( const QString &s )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetTextEvent( 1, s ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setText( s.toStdString() );
  if ( Param->text( 0, "%s" ) != Value )
    Param->addFlags( OW->changedFlag() );
  Value = EW->text().toStdString();
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetText::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetTextEvent *te = dynamic_cast<OptWidgetTextEvent*>( e );
    doTextChanged( te->text() );
  }
  else if ( e->type() == QEvent::User+2 ) {
    browse();
  }
  else if ( e->type() == QEvent::User+3 ) {
    OptWidgetTextEvent *te = dynamic_cast<OptWidgetTextEvent*>( e );
    doBrowse( te->fileName() );
  }
}


void OptWidgetText::initActivation( void )
{
  string s = "";
  if ( EW != 0 )
    s = EW->text().toStdString();
  else
    s = LW->text().toStdString();
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( s ) );
}


void OptWidgetText::browse( void )
{
  if ( OW->updateDisabled() )
    return;

  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetTextEvent( 2 ) );
    return;
  }
  Str file = Param->text( 0 );
  int style = Param->style();
  unlockMutex();
  QFileDialog* fd = new QFileDialog( 0 );
  if ( style & OptWidget::BrowseExisting ) {
    fd->setFileMode( QFileDialog::ExistingFile );
    fd->setWindowTitle( "Open File" );
    fd->setDirectory( file.dir().c_str() );
  }
  else if ( style & OptWidget::BrowseAny ) {
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setWindowTitle( "Save File" );
    fd->setDirectory( file.dir().c_str() );
  }
  else if ( style & OptWidget::BrowseDirectory ) {
    fd->setFileMode( QFileDialog::Directory );
    fd->setWindowTitle( "Choose directory" );
    fd->setDirectory( file.preventSlash().dir().c_str() );
  }
  fd->setFilter( "All (*)" );
  fd->setViewMode( QFileDialog::List );
  if ( fd->exec() == QDialog::Accepted ) {
    QStringList qsl = fd->selectedFiles();
    Str filename = "";
    if ( qsl.size() > 0 )
      filename = qsl[0].toStdString();
    doBrowse( filename );
  }
}


void OptWidgetText::doBrowse( Str filename )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetTextEvent( 3, filename ) );
    return;
  }
  if ( ( Param->style() & OptWidget::BrowseAbsolute ) == 0 )
    filename.stripWorkingPath( 3 );
  if ( ( Param->style() & OptWidget::BrowseDirectory ) )
    filename.provideSlash();
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setText( filename );
  if ( Param->text( 0 ) != Value )
    Param->addFlags( OW->changedFlag() );
  EW->setText( Param->text( 0, "%s" ).c_str() );
  Value = EW->text().toStdString();
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetText::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = l;
  }
}


QPushButton *OptWidgetText::browseButton( void )
{
  return BrowseButton;
}


OptWidgetMultiText::OptWidgetMultiText( Options::iterator param, QWidget *label,
					Options *oo, OptWidget *ow,
					QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, label, oo, ow, mutex ),
    EW( 0 ),
    CI( 0 ),
    Inserted( false ),
    Update( true ),
    Value( "" ),
    LW( 0 )
{
  if ( Editable ) {
    W = EW = new QComboBox( parent );
    EW->setEditable( (Param->style() & OptWidget::SelectText) == 0 );
    if ( (Param->style() & OptWidget::SelectText) > 0 )
      OptWidget::setValueStyle( W, Param->style(), OptWidget::Combo );
    else
      OptWidget::setValueStyle( W, Param->style(), OptWidget::Text );
    EW->setInsertPolicy( QComboBox::InsertAtTop );
    EW->setDuplicatesEnabled( false );
    if ( ( Param->style() & OptWidget::ComboAutoCompletion ) == 0 )
      EW->setCompleter( 0 );
    reset();
    connect( EW, SIGNAL( editTextChanged( const QString& ) ),
	     this, SLOT( insertText( const QString & ) ) );
    Value = EW->itemText( 0 ).toStdString();
    connect( EW, SIGNAL( currentIndexChanged( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
    connect( EW, SIGNAL( activated( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
  }
  else {
    LW = new QLabel( Param->text().c_str(), parent );
    OptWidget::setValueStyle( LW, Param->style(), OptWidget::Window );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetMultiText::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setText( EW->currentText().toStdString() );
    for ( int k=0; k<EW->count(); k++ )
      Param->addText( EW->itemText( k ).toStdString() );
    if ( Param->text( 0 ) != Value )
      Param->addFlags( OW->changedFlag() );
    Value = EW->itemText( 0 ).toStdString();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetMultiText::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    Update = false;
    EW->clear();
    if ( Param->size() > 0 ) {
      string first = Param->text( 0 );
      int firstindex = 0;
      for ( int k=0; k<Param->size(); k++ ) {
	string s = Param->text( k );
	EW->addItem( s.c_str() );
	if ( s == first )
	  firstindex = k;
      }
      if ( firstindex > 0 ) {
	EW->removeItem( 0 );
	EW->setCurrentIndex( firstindex-1 );
      }
      else
	EW->setCurrentIndex( 0 );
    }
    CI = 0;
    Inserted = false;
    Update = true;
  }
  else {
    LW->setText( Param->text( 0 ).c_str() );
  }
  InternChanged = false;
}


void OptWidgetMultiText::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    Update = false;
    EW->setEditText( Param->defaultText().c_str() );
    Update = true;
    InternChanged = false;
  }
}


void OptWidgetMultiText::update( void )
{
  if ( UnitLabel != 0 ) {
    InternChanged = true;
    UnitLabel->setText( Param->outUnit().htmlUnit().c_str() );
    InternChanged = false;
  }
}


void OptWidgetMultiText::textChanged( const QString &s )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable && Update) {
    if ( InternChanged ) {
      Value = EW->itemText( 0 ).toStdString();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setText( Value );
      for ( int k=0; k<EW->count(); k++ )
	Param->addText( EW->itemText( k ).toStdString() );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doTextChanged( s );
  }
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( s.toStdString() ) );
  }
}


class OptWidgetMultiTextEvent : public QEvent
{
public:
  OptWidgetMultiTextEvent( int type, const QString &s )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Text( s ) {};
  QString text( void ) const { return Text; };
private:
  QString Text;
};


void OptWidgetMultiText::doTextChanged( const QString &s )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetMultiTextEvent( 1, s ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setText( s.toStdString() );
  for ( int k=0; k<EW->count(); k++ )
    Param->addText( EW->itemText( k ).toStdString() );
  if ( Param->text( 0 ) != Value )
    Param->addFlags( OW->changedFlag() );
  Value = EW->itemText( 0 ).toStdString();
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetMultiText::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetMultiTextEvent *te = dynamic_cast<OptWidgetMultiTextEvent*>( e );
    doTextChanged( te->text() );
  }
  else if ( e->type() == QEvent::User+2 ) {
    OptWidgetMultiTextEvent *te = dynamic_cast<OptWidgetMultiTextEvent*>( e );
    doInsertText( te->text() );
  }
}


void OptWidgetMultiText::initActivation( void )
{
  string s = "";
  if ( EW != 0 )
    s = EW->currentText().toStdString();
  else
    s = LW->text().toStdString();
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( s ) );
}


void OptWidgetMultiText::insertText( const QString &text )
{
  if ( ! Update || OW->updateDisabled() )
    return;

  doInsertText( text );
}


void OptWidgetMultiText::doInsertText( const QString &text )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetMultiTextEvent( 2, text ) );
    return;
  }
  if ( CI == EW->currentIndex() &&
       ( CI > 0 || EW->currentText() != EW->itemText( 0 ) ) ) {
    if ( ! Inserted ) {
      EW->insertItem( 0, EW->currentText() );
      EW->setCurrentIndex( 0 );
      CI = 0;
      Inserted = true;
    }
    else
      EW->setItemText( 0, EW->currentText() );
  }
  else {
    CI = EW->currentIndex();
    if ( Inserted ) {
      if ( EW->count() > 0 ) {
	EW->removeItem( 0 );
	EW->setCurrentIndex( CI-1 );
	CI = EW->currentIndex();
      }
      Inserted = false;
    }
  }
  unlockMutex();
}


void OptWidgetMultiText::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = l;
  }
}


OptWidgetNumber::OptWidgetNumber( Options::iterator param, QWidget *label,
				  Options *oo, OptWidget *ow,
				  QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, label, oo, ow, mutex ),
    EW( 0 ),
    Value( 0.0 ),
    LW( 0 ),
    LCDW( 0 )
{
  if ( Editable ) {
    double val = Param->number( Param->outUnit() );
    double min = Param->minimum( Param->outUnit() );
    double max = Param->maximum( Param->outUnit() );
    double step = Param->step( Param->outUnit() );
    W = EW = new DoubleSpinBox( parent );
    EW->setRange( min, max );
    EW->setSingleStep( step );
    if ( Param->isNumber() )
      EW->setFormat( Param->format() );
    else {
      EW->setFormat( "%.0f" );
      if ( Param->style() & OptWidget::SpecialInfinite )
	EW->setSpecialValueText( "infinite" );
    }
    EW->setValue( val );
    OptWidget::setValueStyle( W, Param->style(), OptWidget::Text );
    Value = EW->value();
    EW->setKeyboardTracking( false );
    connect( EW, SIGNAL( valueChanged( double ) ),
	     this, SLOT( valueChanged( double ) ) );
  }
  else {
    if ( Param->style() & OptWidget::ValueLCD ) {
      LCDW = new QLCDNumber( parent );
      LCDW->setSegmentStyle( QLCDNumber::Filled );
      LCDW->setSmallDecimalPoint( true );
      LCDW->display( Param->text().c_str() );	
      OptWidget::setValueStyle( LCDW, Param->style(), OptWidget::TextShade );
      // size:
      if ( ( Param->style() & OptWidget::ValueHuge ) == OptWidget::ValueHuge )
	LCDW->setFixedHeight( 16 * LCDW->sizeHint().height() / 10 );
      else if ( Param->style() & OptWidget::ValueLarge )
	LCDW->setFixedHeight( 13 * LCDW->sizeHint().height() / 10 );
      else if ( Param->style() & OptWidget::ValueSmall )
	LCDW->setFixedHeight( 8 * LCDW->sizeHint().height() / 10 );
      W = LCDW;
    }
    else {
      LW = new QLabel( Param->text().c_str(), parent );
      LW->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
      LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
      LW->setLineWidth( 2 );
      OptWidget::setValueStyle( LW, Param->style(), OptWidget::Window );
      LW->setFixedHeight( LW->sizeHint().height() );
      W = LW;
    }
  }
  if ( W->minimumWidth() < W->minimumSizeHint().width() )
    W->setMinimumWidth( W->minimumSizeHint().width() );
}


void OptWidgetNumber::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setNumber( EW->value(), Param->outUnit() );
    if ( fabs( Param->number( Param->outUnit() ) - Value ) > 0.0001*Param->step() )
      Param->addFlags( OW->changedFlag() );
    Value = EW->value();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetNumber::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    EW->setValue( Param->number( Param->outUnit() ) );
  }
  else {
    if ( Param->style() & OptWidget::ValueLCD )
      LCDW->display( Param->text( "", Param->outUnit() ).c_str() );
    else
      LW->setText( Param->text( "", Param->outUnit() ).c_str() );
  }
  if ( W->minimumWidth() < W->minimumSizeHint().width() )
    W->setMinimumWidth( W->minimumSizeHint().width() );
  InternChanged = false;
}


void OptWidgetNumber::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setValue( Param->defaultNumber( Param->outUnit() ) );
    if ( EW->minimumWidth() < EW->minimumSizeHint().width() )
      EW->setMinimumWidth( EW->minimumSizeHint().width() );
    InternChanged = false;
  }
}


void OptWidgetNumber::update( void )
{
  InternChanged = true;
  if ( UnitLabel != 0 )
    UnitLabel->setText( Param->outUnit().htmlUnit().c_str() );
  if ( Editable ) {
    double val = Param->number( Param->outUnit() );
    double min = Param->minimum( Param->outUnit() );
    double max = Param->maximum( Param->outUnit() );
    double step = Param->step( Param->outUnit() );
    EW->setRange( min, max );
    EW->setSingleStep( step );
    if ( Param->isNumber() )
      EW->setFormat( Param->format() );
    else
      EW->setFormat( "%.0f" );
    InternRead = true;
    EW->setValue( val );
    if ( EW->minimumWidth() < EW->minimumSizeHint().width() )
      EW->setMinimumWidth( EW->minimumSizeHint().width() );
    InternRead = false;
  }
  InternChanged = false;
}


void OptWidgetNumber::valueChanged( double v )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable ) {
    if ( InternChanged ) {
      Value = EW->value();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setNumber( Value, Param->outUnit() );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doValueChanged( v );
  }

  if ( W->minimumWidth() < W->minimumSizeHint().width() )
    W->setMinimumWidth( W->minimumSizeHint().width() );

  double tol = 0.2;
  if ( Param->isNumber() )
    tol = 0.01*Param->step();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
     if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( v, tol ) );
  }
}


class OptWidgetNumberEvent : public QEvent
{
public:
  OptWidgetNumberEvent( double value )
    : QEvent( QEvent::Type( QEvent::User+1 ) ), Value( value ) {};
  double value( void ) const { return Value; };
private:
  double Value;
};


void OptWidgetNumber::doValueChanged( double v )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetNumberEvent( v ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setNumber( v, Param->outUnit() );
  if ( fabs( v - Value ) > 0.0001*Param->step() )
    Param->addFlags( OW->changedFlag() );
  Value = EW->value();
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetNumber::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetNumberEvent *ne = dynamic_cast<OptWidgetNumberEvent*>( e );
    doValueChanged( ne->value() );
  }
}


void OptWidgetNumber::initActivation( void )
{
  double v = 0.0;
  if ( EW == 0 && LCDW == 0 )
    v = Param->number( Param->outUnit() );
  else {
    if ( EW != 0 )
      v = EW->value();
    else if ( LCDW != 0 )
      v = LCDW->value();
  }
  double tol = 0.2;
  if ( Param->isNumber() )
    tol = 0.01*Param->step();
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( v, tol ) );
}


void OptWidgetNumber::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = l;
  }
}


OptWidgetBoolean::OptWidgetBoolean( Options::iterator param, Options *oo,
				    OptWidget *ow, const string &request,
				    QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, 0, oo, ow, mutex ),
    EW( 0 ),
    Value( false )
{
  W = new QWidget;
  QHBoxLayout *hb = new QHBoxLayout;
  hb->setContentsMargins( 0, 0, 0, 0 );
  W->setLayout( hb );
  EW = new QCheckBox;
  hb->addWidget( EW );
  OptWidget::setValueStyle( EW, Param->style(), OptWidget::Text );
  QLabel *label = new QLabel( request.c_str() );
  OptWidget::setLabelStyle( label, Param->style() );
  hb->addWidget( label );
  LabelW = label;
  reset();
  if ( Editable ) {
    EW->setEnabled( true );
    Value = EW->isChecked();
    connect( EW, SIGNAL( toggled( bool ) ),
	     this, SLOT( valueChanged( bool ) ) );
  }
  else {
    EW->setEnabled( false );
  }
}


void OptWidgetBoolean::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setBoolean( EW->isChecked() );
    if ( Param->boolean( 0 ) != Value )
      Param->addFlags( OW->changedFlag() );
    Value = EW->isChecked();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetBoolean::reset( void )
{
  InternChanged = true;
  EW->setChecked( Param->boolean() );
  InternChanged = false;
}


void OptWidgetBoolean::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setChecked( Param->defaultBoolean() );
    InternChanged = false;
  }
}


void OptWidgetBoolean::valueChanged( bool v )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable ) {
    if ( InternChanged ) {
      Value = EW->isChecked();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setBoolean( Value );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doValueChanged( v );
  }
  string b( v ? "true" : "false" );
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( b ) );
  }
}


class OptWidgetBooleanEvent : public QEvent
{
public:
  OptWidgetBooleanEvent( bool value )
    : QEvent( QEvent::Type( QEvent::User+1 ) ), Value( value ) {};
  bool value( void ) const { return Value; };
private:
  bool Value;
};


void OptWidgetBoolean::doValueChanged( bool v )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetBooleanEvent( v ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setBoolean( v );
  if ( Param->boolean( 0 ) != Value )
    Param->addFlags( OW->changedFlag() );
  Value = EW->isChecked();
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetBoolean::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetBooleanEvent *be = dynamic_cast<OptWidgetBooleanEvent*>( e );
    doValueChanged( be->value() );
  }
}


void OptWidgetBoolean::initActivation( void )
{
  string b( EW->isChecked() ? "true" : "false" );
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( b ) );
}


OptWidgetDate::OptWidgetDate( Options::iterator param, QWidget *label,
			      Options *oo, OptWidget *ow,
			      QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, label, oo, ow, mutex ),
    DE( 0 ),
    LW( 0 ),
    Year( 0 ),
    Month( 0 ),
    Day( 0 )
{
  if ( Editable ) {
    Year = Param->year( 0 );
    Month = Param->month( 0 );
    Day = Param->day( 0 );
    W = DE = new QDateEdit( QDate( Year, Month, Day ), parent );
    OptWidget::setValueStyle( W, Param->style(), OptWidget::Text );
    DE->setDisplayFormat( "yyyy-MM-dd" );
    Year = DE->date().year();
    Month = DE->date().month();
    Day = DE->date().day();
    connect( DE, SIGNAL( dateChanged( const QDate& ) ),
	     this, SLOT( valueChanged( const QDate& ) ) );
  }
  else {
    LW = new QLabel( Param->text().c_str(), parent );
    OptWidget::setValueStyle( LW, Param->style(), OptWidget::Window );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetDate::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setDate( DE->date().year(), DE->date().month(), DE->date().day() );
    if ( Param->year( 0 ) != Year ||
	 Param->month( 0 ) != Month ||
	 Param->day( 0 ) != Day )
      Param->addFlags( OW->changedFlag() );
    Year = DE->date().year();
    Month = DE->date().month();
    Day = DE->date().day();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetDate::reset( void )
{
  InternChanged = true;
  DE->setDate( QDate( Param->year(), Param->month(), Param->day() ) );
  InternChanged = false;
}


void OptWidgetDate::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    DE->setDate( QDate( Param->defaultYear(),
			Param->defaultMonth(),
			Param->defaultDay() ) );
    InternChanged = false;
  }
}


void OptWidgetDate::valueChanged( const QDate &date )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable ) {
    if ( InternChanged ) {
      Year = DE->date().year();
      Month = DE->date().month();
      Day = DE->date().day();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setDate( Year, Month, Day );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doValueChanged( date );
  }
  string s = "";
  if ( DE != 0 )
    s = DE->date().toString( Qt::ISODate ).toStdString();
  else
    s = LW->text().toStdString();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( s ) );
  }
}


class OptWidgetDateEvent : public QEvent
{
public:
  OptWidgetDateEvent( const QDate &date )
    : QEvent( QEvent::Type( QEvent::User+1 ) ), Date( date ) {};
  QDate date( void ) const { return Date; };
private:
  QDate Date;
};


void OptWidgetDate::doValueChanged( const QDate &date )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetDateEvent( date ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setDate( date.year(), date.month(), date.day() );
  if ( Param->year( 0 ) != Year ||
       Param->month( 0 ) != Month ||
       Param->day( 0 ) != Day )
    Param->addFlags( OW->changedFlag() );
  Year = Param->year( 0 );
  Month = Param->month( 0 );
  Day = Param->day( 0 );
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetDate::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetDateEvent *de = dynamic_cast<OptWidgetDateEvent*>( e );
    doValueChanged( de->date() );
  }
}


void OptWidgetDate::initActivation( void )
{
  string s = "";
  if ( DE != 0 )
    s = DE->date().toString( Qt::ISODate ).toStdString();
  else
    s = LW->text().toStdString();
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( s ) );
}


OptWidgetTime::OptWidgetTime( Options::iterator param, QWidget *label,
			      Options *oo, OptWidget *ow,
			      QMutex *mutex, QWidget *parent )
  : OptWidgetBase( param, label, oo, ow, mutex ),
    TE( 0 ),
    LW( 0 ),
    Hour( 0 ),
    Minutes( 0 ),
    Seconds( 0 )
{
  if ( Editable ) {
    Hour = Param->hour( 0 );
    Minutes = Param->minutes( 0 );
    Seconds = Param->seconds( 0 );
    W = TE = new QTimeEdit( QTime( Hour, Minutes, Seconds ), parent );
    OptWidget::setValueStyle( W, Param->style(), OptWidget::Text );
    TE->setDisplayFormat( "hh:mm:ss" );
    Hour = TE->time().hour();
    Minutes = TE->time().minute();
    Seconds = TE->time().second();
    connect( TE, SIGNAL( timeChanged( const QTime& ) ),
	     this, SLOT( valueChanged( const QTime& ) ) );
  }
  else {
    LW = new QLabel( Param->text().c_str(), parent );
    OptWidget::setValueStyle( LW, Param->style(), OptWidget::Window );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetTime::get( void )
{
  if ( Editable ) {
    InternRead = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    Param->setTime( TE->time().hour(), TE->time().minute(), TE->time().second() );
    if ( Param->hour( 0 ) != Hour ||
	 Param->minutes( 0 ) != Minutes ||
	 Param->seconds( 0 ) != Seconds )
      Param->addFlags( OW->changedFlag() );
    Hour = TE->time().hour();
    Minutes = TE->time().minute();
    Seconds = TE->time().second();
    OO->setNotify( cn );
    InternRead = false;
  }
}


void OptWidgetTime::reset( void )
{
  InternChanged = true;
  TE->setTime( QTime( Param->hour(), Param->minutes(), Param->seconds() ) );
  InternChanged = false;
}


void OptWidgetTime::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    TE->setTime( QTime( Param->defaultHour(),
			Param->defaultMinutes(),
			Param->defaultSeconds() ) );
    InternChanged = false;
  }
}


void OptWidgetTime::valueChanged( const QTime &time )
{
  if ( InternRead || OW->updateDisabled() )
    return;

  if ( ContUpdate && Editable ) {
    if ( InternChanged ) {
      Hour = TE->time().hour();
      Minutes = TE->time().minute();
      Seconds = TE->time().second();
      bool cn = OO->notifying();
      OO->unsetNotify();
      Param->setTime( Hour, Minutes, Seconds );
      Param->delFlags( OW->changedFlag() );
      OO->setNotify( cn );
    }
    else
      doValueChanged( time );
  }
  string s = "";
  if ( TE != 0 )
    s = TE->time().toString( Qt::ISODate ).toStdString();
  else
    s = LW->text().toStdString();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() != OO->end() )
      Widgets[k]->activateOption( Widgets[k]->param()->testActivation( s ) );
  }
}


class OptWidgetTimeEvent : public QEvent
{
public:
  OptWidgetTimeEvent( const QTime &time )
    : QEvent( QEvent::Type( QEvent::User+1 ) ), Time( time ) {};
  QTime time( void ) const { return Time; };
private:
  QTime Time;
};


void OptWidgetTime::doValueChanged( const QTime &time )
{
  if ( ! tryLockMutex( 5 ) ) {
    // we do not get the lock for the data now,
    // so we repost the event to a later time.
    QCoreApplication::postEvent( this, new OptWidgetTimeEvent( time ) );
    return;
  }
  bool cn = OO->notifying();
  OO->unsetNotify();
  Param->setTime( time.hour(), time.minute(), time.second() );
  if ( Param->hour( 0 ) != Hour ||
       Param->minutes( 0 ) != Minutes ||
       Param->seconds( 0 ) != Seconds )
    Param->addFlags( OW->changedFlag() );
  Hour = Param->hour( 0 );
  Minutes = Param->minutes( 0 );
  Seconds = Param->seconds( 0 );
  if ( cn )
    OO->notify();
  if ( ContUpdate )
    Param->delFlags( OW->changedFlag() );
  OO->setNotify( cn );
  unlockMutex();
}


void OptWidgetTime::customEvent( QEvent *e )
{
  if ( e->type() == QEvent::User+1 ) {
    OptWidgetTimeEvent *te = dynamic_cast<OptWidgetTimeEvent*>( e );
    doValueChanged( te->time() );
  }
}


void OptWidgetTime::initActivation( void )
{
  string s = "";
  if ( TE != 0 )
    s = TE->time().toString( Qt::ISODate ).toStdString();
  else
    s = LW->text().toStdString();
  if ( Widgets.back()->param() != OO->end() )
    Widgets.back()->activateOption( Widgets.back()->param()->testActivation( s ) );
}


OptWidgetSection::OptWidgetSection( Options::section_iterator sec,
				    Options *oo, OptWidget *ow,
				    QMutex *mutex, QWidget *parent )
  : OptWidgetBase( oo->end(), 0, oo, ow, mutex ),
    Sec( sec )
{
  Str name = (*Sec)->name();
  string id = ( (*Sec)->style() & OptWidget::MathLabel ) ?
    name.htmlUnit() : name.html();
  QLabel *l = new QLabel( id.c_str(), parent );
  l->setTextFormat( Qt::RichText );
  l->setAlignment( Qt::AlignLeft );
  l->setWordWrap( false );
  W = l;
}


}; /* namespace relacs */

#include "moc_optwidgetbase.cc"

