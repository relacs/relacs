/*
  optwidget.cc
  A Widget for modifying Options.

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

#include <cmath>
#include <QApplication>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QFileDialog>
#include <relacs/optwidget.h>
#include <relacs/optwidgetbase.h>

namespace relacs {


OptWidget::OptWidget( QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( 0 ),
    MaxLines( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
}


OptWidget::OptWidget( Options *o, QMutex *mutex, 
		      QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( mutex ),
    MaxLines( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
  assign( o );
}


OptWidget::OptWidget( Options *o, int selectmask, int romask,
		      bool contupdate, int style, QMutex *mutex, 
		      QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( mutex ),
    MaxLines( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
  assign( o, selectmask, romask, contupdate, style, mutex );
}


OptWidget::~OptWidget( void )
{
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    delete Widgets[k];
  Widgets.clear();
}


OptWidget &OptWidget::operator=( Options *o )
{
  assign( o );
  return *this;
}


OptWidget &OptWidget::assign( Options *o, int selectmask, int romask,
			      bool contupdate, int style, QMutex *mutex )
{
  if ( mutex != 0 )
    OMutex = mutex;

  if ( MainWidget != 0 ) {
    layout()->removeWidget( MainWidget );
    MainWidget->hide();
    delete MainWidget;
    delete layout();
  }
  FirstWidget = 0;
  LastWidget = 0;

  Opt = o;
  Widgets.clear();
  Layout.clear();
  MaxLines = 0;

  if ( OMutex != 0 )
    OMutex->lock();

  if ( Opt->empty() ) {
    if ( OMutex != 0 )
      OMutex->unlock();
    return *this;
  }

  SelectMask = selectmask;
  ReadOnlyMask = romask;
  ContinuousUpdate = contupdate;

  Options::iterator pp = Opt->begin();

  QWidget *parent = this;
  QTabWidget *tabwidget = 0;
  bool tabs = false;
  for ( ; pp != Opt->end(); ++pp ) {
    if ( selectmask <= 0 || ( (*pp).flags() & selectmask ) ) {
      tabs = ( (*pp).isLabel() && 
	       ( (style & TabLabelStyle ) || ( (*pp).style() & TabLabel ) ) );
      break;
    }
  }

  if ( tabs ) {
    QGridLayout *l = new QGridLayout( this );
    tabwidget = new QTabWidget( this );
    l->addWidget( tabwidget, 0, 0 );
    MainWidget = tabwidget;
  }
  else {
    QGridLayout *l = new QGridLayout( this );
    QWidget *w = new QWidget( this );
    l->addWidget( w, 0, 0 );
    MainWidget = w;
    parent = w;
  }

  for ( ; pp != Opt->end(); ) {

    if ( tabs ) {
      QWidget *w = new QWidget( this );
      if ( (*pp).ident().contains( '&' ) )
	tabwidget->addTab( w, (*pp).ident().c_str() );
      else
	tabwidget->addTab( w, ( "&" + (*pp).ident() ).c_str() );
      parent = w;
      for ( ++pp; pp != Opt->end(); ++pp )
	if ( selectmask <= 0 || ( (*pp).flags() & selectmask ) )
	  break;
    }

    int lines = 0;
    for ( Options::iterator op = pp; op != Opt->end(); ++op ) {
      if ( selectmask <= 0 || ( (*pp).flags() & selectmask ) ) {
	if ( tabs && (*op).isLabel() &&
	     ( (style & TabLabelStyle ) || ( (*op).style() & TabLabel ) ) )
	  break;
	lines++;
      }
    }

    Layout.push_back( new QGridLayout( parent ) );
    Layout.back()->setContentsMargins( 0, 0, 0, 0 );
    Layout.back()->setSpacing( 0 );
    QLabel *l;

    int row = 0;
    for ( ; pp != Opt->end(); ) {

      if ( tabs && (*pp).isLabel() &&
	   ( (style & TabLabelStyle ) || ( (*pp).style() & TabLabel ) ) ) {
	QWidget *ll = new QWidget( parent );
	ll->setMinimumHeight( 0 );
	ll->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) );
	Layout.back()->addWidget( ll, row, 1 );
	break;
      }

      bool added = true;
      // request string:
      string rs = (*pp).request();
      if ( style & NameFrontStyle ) {
	if ( style & HighlightNameStyle )
	  rs = "<nobr><tt>" + (*pp).ident() + "</tt>: " + rs + "</nobr>";
	else
	  rs = (*pp).ident() + ": " + rs;
      }
      else if ( style & NameBehindStyle ) {
	if ( style & HighlightNameStyle )
	  rs = "<nobr>" + rs + " <tt>(" + (*pp).ident() + ")</tt></nobr>";
	else
	  rs += " (" + (*pp).ident() + ")";
      }
      // text:
      if ( (*pp).isText() ) {
	l = new QLabel( rs.c_str(), parent );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	QLabel *ul = 0;
	if ( ! (*pp).outUnit().empty() && ( (*pp).style() & Browse ) == 0 ) {
	  ul = unitLabel( *pp, parent );
	  setLabelStyle( ul, (*pp).style() );
	  if ( style & BreakLinesStyle )
	    Layout.back()->addWidget( ul, row+1, 2,
				      Qt::AlignLeft | Qt::AlignVCenter );
	  else
	    Layout.back()->addWidget( ul, row, 3,
				      Qt::AlignLeft | Qt::AlignVCenter );
	}
	if ( (*pp).size() <= 1 ) {
	  OptWidgetText *t = new OptWidgetText( pp, l, Opt, this, parent, OMutex );
	  if ( t->Editable ) {
	    if ( FirstWidget == 0 )
	      FirstWidget = t->W;
	    LastWidget = t->W;
	  }
	  if ( style & BreakLinesStyle ) {
	    row++;
	    if ( ul != 0 || t->browseButton() != 0 )
	      Layout.back()->addWidget( t->W, row, 1 );
	    else
	      Layout.back()->addWidget( t->W, row, 1, 1, 2 );
	  }
	  else
	    Layout.back()->addWidget( t->W, row, 2 );
	  t->setUnitLabel( ul );
	  if ( t->browseButton() != 0 ) {
	    if ( style & BreakLinesStyle )
	      Layout.back()->addWidget( t->browseButton(), row+1, 2,
					Qt::AlignLeft | Qt::AlignVCenter );
	    else
	      Layout.back()->addWidget( t->browseButton(), row, 3,
					Qt::AlignLeft | Qt::AlignVCenter );
	  }
	}
	else {
	  OptWidgetMultiText *t = new OptWidgetMultiText( pp, l, Opt, this, parent, OMutex );
	  if ( t->Editable ) {
	    if ( FirstWidget == 0 )
	      FirstWidget = t->W;
	    LastWidget = t->W;
	  }
	  if ( style & BreakLinesStyle ) {
	    row++;
	    if ( ul != 0 )
	      Layout.back()->addWidget( t->W, row, 1 );
	    else
	      Layout.back()->addWidget( t->W, row, 1, 1, 2 );
	  }
	  else
	    Layout.back()->addWidget( t->W, row, 2 );
	  t->setUnitLabel( ul );
	}
      }
      // number:
      else if ( (*pp).isNumber() || (*pp).isInteger() ) {
	l = new QLabel( rs.c_str(), parent );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetNumber *n = new OptWidgetNumber( pp, l, Opt, this, parent, OMutex );
	if ( n->Editable ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = n->W;
	  LastWidget = n->W;
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( n->W, row, 1 );
	}
	else
	  Layout.back()->addWidget( n->W, row, 2 );
	QLabel *ul = unitLabel( *pp, parent );
	setLabelStyle( ul, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( ul, row, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( ul, row, 3,
				    Qt::AlignLeft | Qt::AlignVCenter );
	n->setUnitLabel( ul );
      }
      // boolean:
      else if ( (*pp).isBoolean() ) {
	OptWidgetBoolean *b = new OptWidgetBoolean( pp, Opt, this, parent, rs, OMutex );
	if ( b->Editable ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = b->W;
	  LastWidget = b->W;
	}
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( b->W, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( b->W, row, 1, 1, 3,
				    Qt::AlignLeft | Qt::AlignVCenter );
      }
      // date:
      else if ( (*pp).isDate() ) {
	l = new QLabel( rs.c_str(), parent );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetDate *d = new OptWidgetDate( pp, Opt, this, parent, rs, OMutex );
	if ( d->Editable ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = d->W;
	  LastWidget = d->W;
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( d->W, row, 1 );
	}
	else
	  Layout.back()->addWidget( d->W, row, 2 );
      }
      // time:
      else if ( (*pp).isTime() ) {
	l = new QLabel( rs.c_str(), parent );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetTime *t = new OptWidgetTime( pp, Opt, this, parent, rs, OMutex );
	if ( t->Editable ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = t->W;
	  LastWidget = t->W;
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( t->W, row, 1 );
	}
	else
	  Layout.back()->addWidget( t->W, row, 2 );
      }
      // label:
      else if ( (*pp).isLabel() ) {
	OptWidgetLabel *l = new OptWidgetLabel( pp, Opt, this, parent, OMutex );
	if ( style & BreakLinesStyle ) {
	  Layout.back()->addWidget( l->W, row, 0, 1, 4,
				    Qt::AlignLeft | Qt::AlignBottom );
	}
	else
	  Layout.back()->addWidget( l->W, row, 0, 1, 5,
				    Qt::AlignLeft | Qt::AlignBottom );
	Layout.back()->setColumnMinimumWidth( 0, 20 );
      }
      // separator:
      else if ( (*pp).isSeparator() ) {
	OptWidgetSeparator *s = new OptWidgetSeparator( pp, Opt, this, parent, OMutex );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( s->W, row, 0, 1, 4 );
	else
	  Layout.back()->addWidget( s->W, row, 0, 1, 5 );
      }
      // nothing:
      else {
	added = false;
	row--;
      }
      // extra space:
      if ( (style & ExtraSpaceStyle) > 0 && added ) {
	row++;
	QWidget *ll = new QWidget( parent );
	ll->setMinimumHeight( 0 );
	ll->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
	Layout.back()->addWidget( ll, row, 1 );
      }
      row++;
      // max line count:
      if ( row > MaxLines ) {
	MaxLines = row;
      }
      // next selected option:
      for ( ++pp; pp != Opt->end(); ++pp )
	if ( selectmask <= 0 || ( (*pp).flags() & selectmask ) )
	  break;

    }

  }

  // init activation/inactivation:
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    string aname = (*Widgets[k]->OP).activationName();
    if ( aname.size() > 0 ) {
      for ( unsigned int i=0; i<Widgets.size(); i++ ) {
	if ( (*Widgets[i]->OP) == aname )
	  Widgets[i]->addActivation( Widgets[k] );
      }
    }
  }

  if ( OMutex != 0 )
    OMutex->unlock();

  MainWidget->show();

  return *this;
}


void OptWidget::setSpacing( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setSpacing( pixel );
}


void OptWidget::setMargin( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setContentsMargins( pixel, pixel, pixel, pixel );
}


QWidget *OptWidget::firstWidget( void ) const
{
  return FirstWidget;
}


QWidget *OptWidget::lastWidget( void ) const
{
  return LastWidget;
}


void OptWidget::setMutex( QMutex *mutex )
{
  OMutex = mutex;
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->setMutex( mutex );
}


void OptWidget::addWidget( OptWidgetBase *owb )
{
  if ( owb != 0 )
    Widgets.push_back( owb );
}


void OptWidget::setLabelFontStyle( QWidget *w, long style )
{
  QFont nf( w->font() );
  QFontInfo fi( w->fontInfo() );

  // size:
  if ( ( style & LabelHuge ) == LabelHuge )
    nf.setPointSizeF( 1.6 * fi.pointSizeF() );
  else if ( style & LabelLarge )
    nf.setPointSizeF( 1.3 * fi.pointSizeF() );
  else if ( style & LabelSmall )
    nf.setPointSizeF( 0.8 * fi.pointSizeF() );

  // style:
  if ( style & LabelBold )
    nf.setBold( true );
  if ( style & LabelItalic )
    nf.setItalic( true );

  w->setFont( nf );
}


void OptWidget::setLabelColorStyle( QWidget *w, long style, bool palette, bool base, bool button )
{
  QPalette::ColorRole bcr = base ? QPalette::Base : QPalette::Window;
  QPalette::ColorRole fcr = base ? QPalette::Text : QPalette::WindowText;

  QColor bg( w->palette().color( QPalette::Active, bcr ) );
  QColor fg( w->palette().color( QPalette::Active, fcr ) );
  
  if ( ( style & LabelBlue ) == LabelBlue )
    fg = Qt::blue;
  else if ( style & LabelRed )
    fg = Qt::red;
  else if ( style & LabelGreen )
    fg = Qt::green;
  
  if ( style & LabelBackBlack )
    bg = Qt::black;
  else if ( style & LabelBackWhite )
    bg = Qt::white;
  
  if ( palette ) {
    w->setPalette( QPalette( fg, bg ) );
  }
  else {
    QPalette qp( w->palette() );
    if ( button )
      qp.setColor( QPalette::Active, QPalette::Button, bg );
    qp.setColor( bcr, bg );
    qp.setColor( fcr, fg );
    w->setPalette( qp );
  }
}


void OptWidget::setLabelStyle( QWidget *w, long style, bool palette, bool base, bool button )
{
  setLabelFontStyle( w, style );
  setLabelColorStyle( w, style, palette, base, button );
}


void OptWidget::setValueFontStyle( QWidget *w, long style )
{
  setLabelFontStyle( w, style >> 12 );
}


void OptWidget::setValueColorStyle( QWidget *w, long style, bool palette, bool base, bool button )
{
  setLabelColorStyle( w, style >> 12, palette, base, button );
}


void OptWidget::setValueStyle( QWidget *w, long style, bool palette, bool base, bool button )
{
  setLabelStyle( w, style >> 12, palette, base, button );
}


QLabel* OptWidget::unitLabel( const Parameter &p, QWidget *parent )
{
  QLabel *l = new QLabel( p.outUnit().htmlUnit().c_str(), parent );
  l->setTextFormat( Qt::RichText );
  l->setAlignment( Qt::AlignLeft );
  l->setWordWrap( false );
  return l;
}


void OptWidget::accept( bool clearchanged )
{
  if ( OMutex != 0 )
    OMutex->lock();

  // get the values:
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->get();

  // notify:
  DisableUpdate = true;
  if ( Opt->notifying() )
    Opt->notify();
  DisableUpdate = false;

  // react to accept:
  emit valuesChanged();

  // clear changedflag:
  if ( clearchanged ) {
    Opt->delFlags( changedFlag() );
  }

  if ( OMutex != 0 )
    OMutex->unlock();
}


void OptWidget::accept( void )
{
  accept( true );
}


void OptWidget::reset( void )
{
  if ( OMutex != 0 )
    OMutex->lock();
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->reset();
  if ( OMutex != 0 )
    OMutex->unlock();
}


void OptWidget::resetDefault( void )
{
  if ( OMutex != 0 )
    OMutex->lock();
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->resetDefault();
  if ( OMutex != 0 )
    OMutex->unlock();
}


void OptWidget::updateValue( const string &ident )
{
  if ( DisableUpdate )
    return;
  UpdateIdentEvent *ue = new UpdateIdentEvent( 1, ident );
  QApplication::postEvent( this, ue );
}


void OptWidget::updateValues( void )
{
  if ( DisableUpdate )
    return;
  UpdateEvent *ue = new UpdateEvent( 2 );
  QApplication::postEvent( this, ue );
}


void OptWidget::updateValues( int flag )
{
  if ( DisableUpdate || Opt == 0 )
    return;
  // save ChangedFlag:
  Opt->addFlags( UpdateFlag, flag );
  UpdateEvent *ue = new UpdateEvent( 3 );
  QApplication::postEvent( this, ue );
}


void OptWidget::updateSettings( const string &ident )
{
  if ( DisableUpdate )
    return;
  UpdateIdentEvent *ue = new UpdateIdentEvent( 4, ident );
  QApplication::postEvent( this, ue );
}


void OptWidget::updateSettings( void )
{
  if ( DisableUpdate )
    return;
  UpdateEvent *ue = new UpdateEvent( 5 );
  QApplication::postEvent( this, ue );
}


void OptWidget::updateSettings( int flag )
{
  if ( DisableUpdate || Opt == 0 )
    return;
  Opt->addFlags( UpdateFlag, flag );
  UpdateEvent *ue = new UpdateEvent( 6 );
  QApplication::postEvent( this, ue );
}


void OptWidget::customEvent( QEvent *e )
{
  if ( e->type() >= QEvent::User+1 && e->type() <= QEvent::User+6 ) {
    if ( OMutex != 0 )
      OMutex->lock();
    if ( e->type() == QEvent::User+1 ) {
      // updateValues( ident )
      UpdateIdentEvent *ue = (UpdateIdentEvent*)e;
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	if ( (*Widgets[k]->OP).ident() == ue->ident() ) {
	  Widgets[k]->reset();
	  break;
	}
      }
    }
    else if ( e->type() == QEvent::User+2 ) {
      // updateValues()
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	Widgets[k]->reset();
      }
    }
    else if ( e->type() == QEvent::User+3 ) {
      // updateValues( flags )
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	if ( (*Widgets[k]->OP).flags( UpdateFlag ) ) {
	  Widgets[k]->reset();
	  (*Widgets[k]->OP).delFlags( UpdateFlag );
	}
      }
    }
    else if ( e->type() == QEvent::User+4 ) {
      // updaeSettings( ident )
      UpdateIdentEvent *ue = (UpdateIdentEvent*)e;
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	if ( (*Widgets[k]->OP).ident() == ue->ident() ) {
	  Widgets[k]->update();
	  break;
	}
      }
    }
    else if ( e->type() == QEvent::User+5 ) {
      // updateSetings()
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	Widgets[k]->update();
      }
    }
    else if ( e->type() == QEvent::User+6 ) {
      // updateSettings( flags )
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	if ( (*Widgets[k]->OP).flags( UpdateFlag ) ) {
	  Widgets[k]->update();
	  (*Widgets[k]->OP).delFlags( UpdateFlag );
	}
      }
    }
    if ( OMutex != 0 )
      OMutex->unlock();
  }
}
   

OptWidgetBase::OptWidgetBase( Options::iterator op, QWidget *label,
			      Options *oo, OptWidget *ow, QMutex *mutex )
  : QObject(),
    OP( op ),
    OO( oo ),
    OW( ow ),
    LabelW( label ),
    W( 0 ),
    UnitBrowseW( 0 ),
    OMutex( mutex ),
    Editable( true ),
    ContUpdate( ow->continuousUpdate() ),
    InternChanged( false )
{
  if ( OW->readOnlyMask() < 0 ||
       ( OW->readOnlyMask() > 0 && ( (*op).flags() & OW->readOnlyMask() ) ) )
    Editable = false;
  (*OP).delFlags( OW->changedFlag() );
  ow->addWidget( this );
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
  bool ac = (*OP).activation() ? eq : !eq;

  if ( LabelW != 0 )
    LabelW->setEnabled( ac );
  if ( W != 0 )
    W->setEnabled( ac );
  if ( UnitBrowseW != 0 )
    UnitBrowseW->setEnabled( ac );
}


OptWidgetBase::~OptWidgetBase( void )
{
}


OptWidgetText::OptWidgetText( Options::iterator op, QWidget *label,
			      Options *oo, OptWidget *ow,
			      QWidget *parent, QMutex *mutex )
  : OptWidgetBase( op, label, oo, ow, mutex ),
    EW( 0 ),
    Value( "" ),
    LW( 0 ),
    UnitLabel( 0 ),
    BrowseButton( 0 )
{
  if ( Editable ) {
    W = EW = new QLineEdit( (*OP).text().c_str(), parent );
    OptWidget::setValueStyle( W, (*OP).style(), false, true );
    Value = (*OP).text( 0 );
    connect( EW, SIGNAL( textChanged( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
    if ( (*OP).style() & OptWidget::Browse ) {
      BrowseButton = new QPushButton( "Browse...", parent );
      UnitBrowseW = BrowseButton;
      connect( BrowseButton, SIGNAL( clicked( void ) ),
	       this, SLOT( browse( void ) ) );
    }
  }
  else {
    LW = new QLabel( (*OP).text().c_str(), parent );
    OptWidget::setValueStyle( LW, (*OP).style() );
    //    LW->setBackgroundMode( QWidget::PaletteMid );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetText::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setText( EW->text().toLatin1().data() );
    if ( (*OP).text( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).text( 0 );
    OO->setNotify( cn );
  }
}


void OptWidgetText::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    EW->setText( (*OP).text( 0 ).c_str() );
  }
  else if ( ContUpdate ) {
    LW->setText( (*OP).text( 0 ).c_str() );
  }
  InternChanged = false;
}


void OptWidgetText::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setText( (*OP).defaultText().c_str() );
    InternChanged = false;
  }
}


void OptWidgetText::update( void )
{
  if ( UnitLabel != 0 ) {
    InternChanged = true;
    UnitLabel->setText( (*OP).outUnit().c_str() );
    InternChanged = false;
  }
}


void OptWidgetText::textChanged( const QString &s )
{
  if ( ContUpdate && Editable ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setText( s.toLatin1().data() );
    if ( (*OP).text( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).text( 0 );
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( s.toLatin1().data() ) );
  }
}


void OptWidgetText::initActivation( void )
{
  string s = "";
  if ( EW != 0 )
    s = EW->text().toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( s ) );
}


void OptWidgetText::browse( void )
{
  if ( OMutex != 0 )
    OMutex->lock();
  QFileDialog* fd = new QFileDialog( 0 );
  if ( (*OP).style() & OptWidget::BrowseExisting ) {
    fd->setFileMode( QFileDialog::ExistingFile );
    fd->setWindowTitle( "Open File" );
    fd->setDirectory( Str( (*OP).text( 0 ) ).dir().c_str() );
  }
  else if ( (*OP).style() & OptWidget::BrowseAny ) {
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setWindowTitle( "Save File" );
    fd->setDirectory( Str( (*OP).text( 0 ) ).dir().c_str() );
  }
  else if ( (*OP).style() & OptWidget::BrowseDirectory ) {
    fd->setFileMode( QFileDialog::Directory );
    fd->setWindowTitle( "Choose directory" );
    fd->setDirectory( Str( (*OP).text( 0 ) ).preventSlash().dir().c_str() );
  }
  fd->setFilter( "All (*)" );
  fd->setViewMode( QFileDialog::List );
  if ( OMutex != 0 )
    OMutex->unlock();
  if ( fd->exec() == QDialog::Accepted ) {
    if ( OMutex != 0 )
      OMutex->lock();
    Str filename = "";
    QStringList qsl = fd->selectedFiles();
    if ( qsl.size() > 0 )
      Str filename = qsl[0].toLatin1().data();
    if ( ( (*OP).style() & OptWidget::BrowseAbsolute ) == 0 )
      filename.stripWorkingPath( 3 );
    if ( ( (*OP).style() & OptWidget::BrowseDirectory ) )
      filename.provideSlash();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setText( filename );
    if ( (*OP).text( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).text( 0 );
    EW->setText( (*OP).text( 0 ).c_str() );
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 )
      OMutex->unlock();
  }
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


OptWidgetMultiText::OptWidgetMultiText( Options::iterator op, QWidget *label,
					Options *oo, OptWidget *ow,
					QWidget *parent, QMutex *mutex )
  : OptWidgetBase( op, label, oo, ow, mutex ),
    EW( 0 ),
    CI( 0 ),
    Inserted( false ),
    Update( true ),
    Value( "" ),
    LW( 0 ),
    UnitLabel( 0 )
{
  if ( Editable ) {
    W = EW = new QComboBox( parent );
    EW->setEditable( ((*OP).style() & OptWidget::SelectText) == 0 );
    OptWidget::setValueStyle( W, (*OP).style(), false, true, true );
    EW->setInsertPolicy( QComboBox::InsertAtTop );
    EW->setDuplicatesEnabled( false );
    if ( (*OP).style() & OptWidget::ComboAutoCompletion == 0 )
      EW->setCompleter( 0 );
    reset();
    connect( EW, SIGNAL( editTextChanged( const QString& ) ),
	     this, SLOT( insertText( const QString & ) ) );
    Value = (*OP).text( 0 );
    connect( EW, SIGNAL( currentIndexChanged( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
    connect( EW, SIGNAL( activated( const QString& ) ),
	     this, SLOT( textChanged( const QString& ) ) );
  }
  else {
    LW = new QLabel( (*OP).text().c_str(), parent );
    OptWidget::setValueStyle( LW, (*OP).style() );
    //    LW->setBackgroundMode( QWidget::PaletteMid );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetMultiText::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setText( EW->currentText().toLatin1().data() );
    for ( int k=0; k<EW->count(); k++ )
      (*OP).addText( EW->itemText( k ).toLatin1().data() );
    if ( (*OP).text( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).text( 0 );
    OO->setNotify( cn );
  }
}


void OptWidgetMultiText::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    Update = false;
    EW->clear();
    if ( (*OP).size() > 0 ) {
      string first = (*OP).text( 0 );
      int firstindex = 0;
      for ( int k=0; k<(*OP).size(); k++ ) {
	string s = (*OP).text( k );
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
  else if ( ContUpdate ) {
    LW->setText( (*OP).text( 0 ).c_str() );
  }
  InternChanged = false;
}


void OptWidgetMultiText::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    Update = false;
    EW->setEditText( (*OP).defaultText().c_str() );
    Update = true;
    InternChanged = false;
  }
}


void OptWidgetMultiText::update( void )
{
  if ( UnitLabel != 0 ) {
    InternChanged = true;
    UnitLabel->setText( (*OP).outUnit().c_str() );
    InternChanged = false;
  }
}


void OptWidgetMultiText::textChanged( const QString &s )
{
  if ( ContUpdate && Editable && Update ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setText( s.toLatin1().data() );
    for ( int k=0; k<EW->count(); k++ )
      (*OP).addText( EW->itemText( k ).toLatin1().data() );
    if ( (*OP).text( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).text( 0 );
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( s.toLatin1().data() ) );
  }
}


void OptWidgetMultiText::initActivation( void )
{
  string s = "";
  if ( EW != 0 )
    s = EW->currentText().toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( s ) );
}


void OptWidgetMultiText::insertText( const QString &text )
{
  if ( ! Update )
    return;

  if ( OMutex != 0 )
    OMutex->lock();
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
  if ( OMutex != 0 )
    OMutex->unlock();
}


void OptWidgetMultiText::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = l;
  }
}


OptWidgetNumber::OptWidgetNumber( Options::iterator op, QWidget *label,
				  Options *oo, OptWidget *ow,
				  QWidget *parent, QMutex *mutex )
  : OptWidgetBase( op, label, oo, ow, mutex ),
    EW( 0 ),
    Value( 0.0 ),
    LW( 0 ),
    LCDW( 0 ),
    UnitLabel( 0 )
{
  if ( Editable ) {
    double val = (*OP).number( (*OP).outUnit() );
    double min = (*OP).minimum( (*OP).outUnit() );
    double max = (*OP).maximum( (*OP).outUnit() );
    double step = (*OP).step( (*OP).outUnit() );
    int prec = 0;
    if ( (*OP).isNumber() ) {
      int width=0;
      int precision=0;
      char type='g';
      char pad='0';
      Str frmt = (*OP).format();
      frmt.readFormat( 0, width, precision, type, pad );
      int stepprec = (int)-floor( log10( 0.1*step ) );
      if ( precision > stepprec || ( precision >= 0 && type == 'f' ) )
	prec = precision;
      else
	prec = stepprec;
    }
    W = EW = new QDoubleSpinBox( parent );
    EW->setRange( min, max );
    EW->setSingleStep( step );
    EW->setDecimals( prec );
    EW->setValue( val );
    OptWidget::setValueStyle( W, (*OP).style(), false, true );
    Value = (*OP).number();
    connect( EW, SIGNAL( valueChanged( double ) ),
	     this, SLOT( valueChanged( double ) ) );
  }
  else {
    if ( (*OP).style() & OptWidget::ValueLCD ) {
      LCDW = new QLCDNumber( parent );
      LCDW->setSmallDecimalPoint( true );
      LCDW->display( (*OP).text().c_str() );	
      OptWidget::setValueStyle( LCDW, (*OP).style(), true );
      // size: XXX ??
      if ( ( (*OP).style() & OptWidget::ValueHuge ) == OptWidget::ValueHuge )
	LCDW->setMinimumHeight( int( 2.0 * LCDW->height() ) );
      else if ( (*OP).style() & OptWidget::ValueLarge )
	LCDW->setMinimumHeight( int( 1.5 * LCDW->height() ) );
      else if ( (*OP).style() & OptWidget::ValueSmall )
	LCDW->setMinimumHeight( int( 0.8 * LCDW->height() ) );
      W = LCDW;
    }
    else {
      LW = new QLabel( (*OP).text().c_str(), parent );
      //    LW->setBackgroundMode( QWidget::PaletteMid );
      LW->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
      LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
      LW->setLineWidth( 2 );
      OptWidget::setValueStyle( LW, (*OP).style() );
      LW->setFixedHeight( LW->sizeHint().height() );
      W = LW;
    }
  }
}


void OptWidgetNumber::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setNumber( EW->value(), (*OP).outUnit() );
    if ( fabs( (*OP).number( 0 ) - Value ) > 0.0001*(*OP).step() ) {
      (*OP).addFlags( OW->changedFlag() );
    }
    Value = (*OP).number();
    OO->setNotify( cn );
  }
}


void OptWidgetNumber::reset( void )
{
  InternChanged = true;
  if ( Editable ) {
    EW->setValue( (*OP).number( (*OP).outUnit() ) );
  }
  else if ( ContUpdate ) {
    if ( (*OP).style() & OptWidget::ValueLCD )
      LCDW->display( (*OP).text( "", (*OP).outUnit() ).c_str() );
    else
      LW->setText( (*OP).text( "", (*OP).outUnit() ).c_str() );
  }
  InternChanged = false;
}


void OptWidgetNumber::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setValue( (*OP).defaultNumber( (*OP).outUnit() ) );
    InternChanged = false;
  }
}


void OptWidgetNumber::update( void )
{
  InternChanged = true;
  if ( UnitLabel != 0 )
    UnitLabel->setText( (*OP).outUnit().c_str() );
  if ( Editable ) {
    double val = (*OP).number( (*OP).outUnit() );
    double min = (*OP).minimum( (*OP).outUnit() );
    double max = (*OP).maximum( (*OP).outUnit() );
    double step = (*OP).step( (*OP).outUnit() );
    int prec = 0;
    if ( (*OP).isNumber() ) {
      int width=0;
      int precision=0;
      char type='g';
      char pad='0';
      Str frmt = (*OP).format();
      frmt.readFormat( 0, width, precision, type, pad );
      int stepprec = (int)-floor( log10( 0.1*step ) );
      if ( precision > stepprec || ( precision >= 0 && type == 'f' ) )
	prec = precision;
      else
	prec = stepprec;
    }
    EW->setRange( min, max );
    EW->setSingleStep( step );
    EW->setDecimals( prec );
    EW->setValue( val );
  }
  InternChanged = false;
}


void OptWidgetNumber::valueChanged( double v )
{
  if ( ContUpdate && Editable ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setNumber( v, (*OP).outUnit() );
    if ( fabs( (*OP).number( 0 ) - Value ) > 0.0001*(*OP).step() ) {
      (*OP).addFlags( OW->changedFlag() );
    }
    Value = (*OP).number();
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }

  double tol = 0.2;
  if ( (*OP).isNumber() )
    tol = 0.01*(*OP).step();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( v, tol ) );
  }
}


void OptWidgetNumber::initActivation( void )
{
  double v = 0.0;
  if ( EW == 0 && LCDW == 0 )
    v = (*OP).number();
  else {
    if ( EW != 0 )
      v = EW->value();
    else if ( LCDW != 0 )
      v = LCDW->value();
  }
  double tol = 0.2;
  if ( (*OP).isNumber() )
    tol = 0.01*(*OP).step();
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( v, tol ) );
}


void OptWidgetNumber::setUnitLabel( QLabel *l )
{
  if ( l != 0 ) {
    UnitLabel = l;
    UnitBrowseW = l;
  }
}


OptWidgetBoolean::OptWidgetBoolean( Options::iterator op, Options *oo,
				    OptWidget *ow, QWidget *parent,
				    const string &request, QMutex *mutex )
  : OptWidgetBase( op, 0, oo, ow, mutex ),
    EW( 0 ),
    Value( false )
{
  QWidget *hb = new QWidget( parent );
  QHBoxLayout *layout = new QHBoxLayout;
  hb->setLayout( layout );
  W = hb;
  EW = new QCheckBox( " " );
  layout->addWidget( EW );
  QLabel *label = new QLabel( request.c_str() );
  layout->addWidget( label );
  OptWidget::setLabelStyle( label, (*OP).style() );
  reset();
  if ( Editable ) {
    label->setFocusProxy( EW );
    Value = (*OP).boolean();
    connect( EW, SIGNAL( toggled( bool ) ),
	     this, SLOT( valueChanged( bool ) ) );
  }
  else {
    EW->setFocusPolicy( Qt::NoFocus );
    connect( EW, SIGNAL( toggled( bool ) ), this, SLOT( dontToggle( bool ) ) );
  }
}


void OptWidgetBoolean::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setBoolean( EW->isChecked() );
    if ( (*OP).boolean( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).boolean();
    OO->setNotify( cn );
  }
}


void OptWidgetBoolean::reset( void )
{
  InternChanged = true;
  EW->setChecked( (*OP).boolean() );
  InternChanged = false;
}


void OptWidgetBoolean::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    EW->setChecked( (*OP).defaultBoolean() );
    InternChanged = false;
  }
}


void OptWidgetBoolean::valueChanged( bool v )
{
  if ( ContUpdate && Editable ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setBoolean( v );
    if ( (*OP).boolean( 0 ) != Value )
      (*OP).addFlags( OW->changedFlag() );
    Value = (*OP).boolean();
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }
  string b( v ? "true" : "false" );
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( b ) );
  }
}


void OptWidgetBoolean::initActivation( void )
{
  string b( EW->isChecked() ? "true" : "false" );
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( b ) );
}


void OptWidgetBoolean::dontToggle( bool t )
{
  if ( OMutex != 0 )
    OMutex->lock();
  reset();
  if ( OMutex != 0 )
    OMutex->unlock();
}


OptWidgetDate::OptWidgetDate( Options::iterator op, Options *oo,
			      OptWidget *ow, QWidget *parent,
			      const string &request, QMutex *mutex )
  : OptWidgetBase( op, 0, oo, ow, mutex ),
    DE( 0 ),
    LW( 0 ),
    Year( 0 ),
    Month( 0 ),
    Day( 0 )
{
  if ( Editable ) {
    Year = (*OP).year( 0 );
    Month = (*OP).month( 0 );
    Day = (*OP).day( 0 );
    W = DE = new QDateEdit( QDate( Year, Month, Day ), parent );
    OptWidget::setValueStyle( W, (*OP).style(), false, true );
    DE->setDisplayFormat( "yyyy-MM-dd" );
    connect( DE, SIGNAL( dateChanged( const QDate& ) ),
	     this, SLOT( valueChanged( const QDate& ) ) );
  }
  else {
    LW = new QLabel( (*OP).text().c_str(), parent );
    OptWidget::setValueStyle( LW, (*OP).style() );
    //    LW->setBackgroundMode( QWidget::PaletteMid );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetDate::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setDate( DE->date().year(), DE->date().month(), DE->date().day() );
    if ( (*OP).year( 0 ) != Year ||
	 (*OP).month( 0 ) != Month ||
	 (*OP).day( 0 ) != Day )
      (*OP).addFlags( OW->changedFlag() );
    Year = (*OP).year( 0 );
    Month = (*OP).month( 0 );
    Day = (*OP).day( 0 );
    OO->setNotify( cn );
  }
}


void OptWidgetDate::reset( void )
{
  InternChanged = true;
  DE->setDate( QDate( (*OP).year(), (*OP).month(), (*OP).day() ) );
  InternChanged = false;
}


void OptWidgetDate::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    DE->setDate( QDate( (*OP).defaultYear(),
			(*OP).defaultMonth(),
			(*OP).defaultDay() ) );
    InternChanged = false;
  }
}


void OptWidgetDate::valueChanged( const QDate &date )
{
  if ( ContUpdate && Editable ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setDate( date.year(), date.month(), date.day() );
    if ( (*OP).year( 0 ) != Year ||
	 (*OP).month( 0 ) != Month ||
	 (*OP).day( 0 ) != Day )
      (*OP).addFlags( OW->changedFlag() );
    Year = (*OP).year( 0 );
    Month = (*OP).month( 0 );
    Day = (*OP).day( 0 );
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }
  string s = "";
  if ( DE != 0 )
    s = DE->date().toString( Qt::ISODate ).toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( s ) );
  }
}


void OptWidgetDate::initActivation( void )
{
  string s = "";
  if ( DE != 0 )
    s = DE->date().toString( Qt::ISODate ).toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( s ) );
}


OptWidgetTime::OptWidgetTime( Options::iterator op, Options *oo,
			      OptWidget *ow, QWidget *parent,
			      const string &request, QMutex *mutex )
  : OptWidgetBase( op, 0, oo, ow, mutex ),
    TE( 0 ),
    LW( 0 ),
    Hour( 0 ),
    Minutes( 0 ),
    Seconds( 0 )
{
  if ( Editable ) {
    Hour = (*OP).hour( 0 );
    Minutes = (*OP).minutes( 0 );
    Seconds = (*OP).seconds( 0 );
    W = TE = new QTimeEdit( QTime( Hour, Minutes, Seconds ), parent );
    OptWidget::setValueStyle( W, (*OP).style(), false, true );
    TE->setDisplayFormat( "hh:mm:ss" );
    connect( TE, SIGNAL( timeChanged( const QTime& ) ),
	     this, SLOT( valueChanged( const QTime& ) ) );
  }
  else {
    LW = new QLabel( (*OP).text().c_str(), parent );
    OptWidget::setValueStyle( LW, (*OP).style() );
    //    LW->setBackgroundMode( QWidget::PaletteMid );
    LW->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    LW->setLineWidth( 2 );
    W = LW;
  }
}


void OptWidgetTime::get( void )
{
  if ( Editable ) {
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setTime( TE->time().hour(), TE->time().minute(), TE->time().second() );
    if ( (*OP).hour( 0 ) != Hour ||
	 (*OP).minutes( 0 ) != Minutes ||
	 (*OP).seconds( 0 ) != Seconds )
      (*OP).addFlags( OW->changedFlag() );
    Hour = (*OP).hour( 0 );
    Minutes = (*OP).minutes( 0 );
    Seconds = (*OP).seconds( 0 );
    OO->setNotify( cn );
  }
}


void OptWidgetTime::reset( void )
{
  InternChanged = true;
  TE->setTime( QTime( (*OP).hour(), (*OP).minutes(), (*OP).seconds() ) );
  InternChanged = false;
}


void OptWidgetTime::resetDefault( void )
{
  if ( Editable ) {
    InternChanged = true;
    TE->setTime( QTime( (*OP).defaultHour(),
			(*OP).defaultMinutes(),
			(*OP).defaultSeconds() ) );
    InternChanged = false;
  }
}


void OptWidgetTime::valueChanged( const QTime &time )
{
  if ( ContUpdate && Editable ) {
    if ( OMutex != 0 && ! InternChanged )
      OMutex->lock();
    OW->DisableUpdate = true;
    bool cn = OO->notifying();
    OO->unsetNotify();
    (*OP).setTime( time.hour(), time.minute(), time.second() );
    if ( (*OP).hour( 0 ) != Hour ||
	 (*OP).minutes( 0 ) != Minutes ||
	 (*OP).seconds( 0 ) != Seconds )
      (*OP).addFlags( OW->changedFlag() );
    Hour = (*OP).hour( 0 );
    Minutes = (*OP).minutes( 0 );
    Seconds = (*OP).seconds( 0 );
    if ( cn )
      OO->notify();
    (*OP).delFlags( OW->changedFlag() );
    OO->setNotify( cn );
    OW->DisableUpdate = false;
    if ( OMutex != 0 && ! InternChanged )
      OMutex->unlock();
  }
  string s = "";
  if ( TE != 0 )
    s = TE->time().toString( Qt::ISODate ).toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    Widgets[k]->activateOption( (*Widgets[k]->OP).testActivation( s ) );
  }
}


void OptWidgetTime::initActivation( void )
{
  string s = "";
  if ( TE != 0 )
    s = TE->time().toString( Qt::ISODate ).toLatin1().data();
  else
    s = LW->text().toLatin1().data();
  Widgets.back()->activateOption( (*Widgets.back()->OP).testActivation( s ) );
}


OptWidgetLabel::OptWidgetLabel( Options::iterator op, Options *oo,
				OptWidget *ow, QWidget *parent, QMutex *mutex )
  : OptWidgetBase( op, 0, oo, ow, mutex )
{
  string id = ( (*OP).style() & OptWidget::MathLabel ) ?
    (*OP).ident().htmlUnit() : (*OP).ident().html();
  QLabel *l = new QLabel( id.c_str(), parent );
  l->setTextFormat( Qt::RichText );
  l->setAlignment( Qt::AlignLeft );
  l->setWordWrap( false );
  W = l;
  OptWidget::setLabelStyle( W, (*OP).style() );
}


OptWidgetSeparator::OptWidgetSeparator( Options::iterator op, Options *oo,
					OptWidget *ow, QWidget *parent, QMutex *mutex )
  : OptWidgetBase( op, 0, oo, ow, mutex )
{
  QWidget *h = new QWidget( parent );
  QHBoxLayout *layout = new QHBoxLayout;
  h->setLayout( layout );
  W = h;
  if ( (*OP).ident().empty() ) {
    QLabel *line = new QLabel;
    layout->addWidget( line );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setLineWidth( 1 );
    line->setMidLineWidth( 0 );
    line->setFixedHeight( 4 );
  }
  else {
    QLabel *line;
    line = new QLabel;
    layout->addWidget( line );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setLineWidth( 1 );
    line->setMidLineWidth( 0 );
    line->setFixedHeight( 4 );
    layout->setStretchFactor( line, 10 );
    QLabel *label = new QLabel( (*OP).ident().c_str() );
    layout->addWidget( label );
    OptWidget::setLabelStyle( label, (*OP).style() );
    label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    int w = label->sizeHint().width();
    label->setFixedWidth( w + 4*w/(*OP).ident().size() );
    layout->setStretchFactor( label, 1 );
    line = new QLabel;
    layout->addWidget( line );
    line->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    line->setLineWidth( 1 );
    line->setMidLineWidth( 0 );
    line->setFixedHeight( 4 );
    layout->setStretchFactor( line, 30 );
  }
}


}; /* namespace relacs */

#include "moc_optwidget.cc"

