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

#include <QApplication>
#include <QVBoxLayout>
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
    delete layout();
    delete MainWidget;
  }
  MainWidget = 0;
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

  MainWidget = this;
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

  QVBoxLayout *l = new QVBoxLayout;
  l->setSpacing( 0 );
  l->setContentsMargins( 0, 0, 0, 0 );
  parent->setLayout( l );
  if ( tabs ) {
    tabwidget = new QTabWidget;
    l->addWidget( tabwidget );
    MainWidget = tabwidget;
  }
  else {
    QWidget *w = new QWidget;
    l->addWidget( w );
    MainWidget = w;
    parent = w;
  }

  while ( pp != Opt->end() ) {

    if ( tabs ) {
      QWidget *w = new QWidget;
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

    Layout.push_back( new QGridLayout );
    parent->setLayout( Layout.back() );
    QLabel *l;

    int row = 0;
    while ( pp != Opt->end() ) {

      if ( tabs && (*pp).isLabel() &&
	   ( (style & TabLabelStyle ) || ( (*pp).style() & TabLabel ) ) ) {
	if ( (style & ExtraSpaceStyle) == 0 )
	  Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), row, 2 );
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
	l = new QLabel( rs.c_str() );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	QLabel *ul = 0;
	if ( ! (*pp).outUnit().empty() && ( (*pp).style() & Browse ) == 0 ) {
	  ul = unitLabel( *pp );
	  setLabelStyle( ul, (*pp).style() );
	  if ( style & BreakLinesStyle )
	    Layout.back()->addWidget( ul, row+1, 2,
				      Qt::AlignLeft | Qt::AlignVCenter );
	  else
	    Layout.back()->addWidget( ul, row, 3,
				      Qt::AlignLeft | Qt::AlignVCenter );
	}
	if ( (*pp).size() <= 1 ) {
	  OptWidgetText *t = new OptWidgetText( pp, l, Opt, this, OMutex );
	  if ( t->editable() ) {
	    if ( FirstWidget == 0 )
	      FirstWidget = t->valueWidget();
	    LastWidget = t->valueWidget();
	  }
	  if ( style & BreakLinesStyle ) {
	    row++;
	    if ( ul != 0 || t->browseButton() != 0 )
	      Layout.back()->addWidget( t->valueWidget(), row, 1 );
	    else
	      Layout.back()->addWidget( t->valueWidget(), row, 1, 1, 2 );
	  }
	  else
	    Layout.back()->addWidget( t->valueWidget(), row, 2 );
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
	  OptWidgetMultiText *t = new OptWidgetMultiText( pp, l, Opt, this, OMutex );
	  if ( t->editable() ) {
	    if ( FirstWidget == 0 )
	      FirstWidget = t->valueWidget();
	    LastWidget = t->valueWidget();
	  }
	  if ( style & BreakLinesStyle ) {
	    row++;
	    if ( ul != 0 )
	      Layout.back()->addWidget( t->valueWidget(), row, 1 );
	    else
	      Layout.back()->addWidget( t->valueWidget(), row, 1, 1, 2 );
	  }
	  else
	    Layout.back()->addWidget( t->valueWidget(), row, 2 );
	  t->setUnitLabel( ul );
	}
      }
      // number:
      else if ( (*pp).isNumber() || (*pp).isInteger() ) {
	l = new QLabel( rs.c_str() );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetNumber *n = new OptWidgetNumber( pp, l, Opt, this, OMutex );
	if ( n->editable() ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = n->valueWidget();
	  LastWidget = n->valueWidget();
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( n->valueWidget(), row, 1 );
	}
	else
	  Layout.back()->addWidget( n->valueWidget(), row, 2 );
	QLabel *ul = unitLabel( *pp );
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
	OptWidgetBoolean *b = new OptWidgetBoolean( pp, Opt, this, rs, OMutex );
	if ( b->editable() ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = b->valueWidget();
	  LastWidget = b->valueWidget();
	}
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( b->valueWidget(), row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else {
	  Layout.back()->addWidget( b->valueWidget(), row, 1, 1, 3,
				    Qt::AlignLeft | Qt::AlignVCenter );
	  // the following is necessary to eliminate expanding space on top of the boolean:
	  QLabel *spacer = new QLabel;
	  spacer->setFixedHeight( b->valueWidget()->sizeHint().height() );
	  Layout.back()->addWidget( spacer, row, 2 );
	}
      }
      // date:
      else if ( (*pp).isDate() ) {
	l = new QLabel( rs.c_str() );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetDate *d = new OptWidgetDate( pp, Opt, this, OMutex );
	if ( d->editable() ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = d->valueWidget();
	  LastWidget = d->valueWidget();
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( d->valueWidget(), row, 1 );
	}
	else
	  Layout.back()->addWidget( d->valueWidget(), row, 2 );
      }
      // time:
      else if ( (*pp).isTime() ) {
	l = new QLabel( rs.c_str() );
	setLabelStyle( l, (*pp).style() );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( l, row, 1, 1, 2,
				    Qt::AlignLeft | Qt::AlignVCenter );
	else
	  Layout.back()->addWidget( l, row, 1,
				    Qt::AlignLeft | Qt::AlignVCenter );
	OptWidgetTime *t = new OptWidgetTime( pp, Opt, this, OMutex );
	if ( t->editable() ) {
	  if ( FirstWidget == 0 )
	    FirstWidget = t->valueWidget();
	  LastWidget = t->valueWidget();
	}
	if ( style & BreakLinesStyle ) {
	  row++;
	  Layout.back()->addWidget( t->valueWidget(), row, 1 );
	}
	else
	  Layout.back()->addWidget( t->valueWidget(), row, 2 );
      }
      // label:
      else if ( (*pp).isLabel() ) {
	OptWidgetLabel *l = new OptWidgetLabel( pp, Opt, this, OMutex );
	if ( style & BreakLinesStyle ) {
	  Layout.back()->addWidget( l->valueWidget(), row, 0, 1, 4,
				    Qt::AlignLeft | Qt::AlignBottom );
	}
	else {
	  Layout.back()->addWidget( l->valueWidget(), row, 0, 1, 5,
				    Qt::AlignLeft | Qt::AlignBottom );
	  // the following is necessary to eliminate expanding space on top of the label:
	  QLabel *spacer = new QLabel;
	  spacer->setFixedHeight( l->valueWidget()->sizeHint().height() );
	  Layout.back()->addWidget( spacer, row, 2 );
	}
	Layout.back()->setColumnMinimumWidth( 0, 20 );
      }
      // separator:
      else if ( (*pp).isSeparator() ) {
	OptWidgetSeparator *s = new OptWidgetSeparator( pp, Opt, this, OMutex );
	if ( style & BreakLinesStyle )
	  Layout.back()->addWidget( s->valueWidget(), row, 0, 1, 4 );
	else
	  Layout.back()->addWidget( s->valueWidget(), row, 0, 1, 5 );
      }
      // nothing:
      else {
	added = false;
	row--;
      }
      // extra space:
      if ( (style & ExtraSpaceStyle) > 0 && added ) {
	row++;
	Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), row, 2 );
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

    if ( row > 0 && (style & ExtraSpaceStyle) == 0 && pp == Opt->end() )
      Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum, QSizePolicy::Expanding ), row, 2 );

  }

  // init activation/inactivation:
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    string aname = Widgets[k]->param().activationName();
    if ( aname.size() > 0 ) {
      for ( unsigned int i=0; i<Widgets.size(); i++ ) {
	if ( Widgets[i]->param() == aname )
	  Widgets[i]->addActivation( Widgets[k] );
      }
    }
  }

  if ( OMutex != 0 )
    OMutex->unlock();

  MainWidget->show();

  return *this;
}


int OptWidget::lines( void ) const
{
  return MaxLines;
}


void OptWidget::setVerticalSpacing( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setVerticalSpacing( pixel );
}


void OptWidget::setHorizontalSpacing( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setHorizontalSpacing( pixel );
}


void OptWidget::setMargins( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setContentsMargins( pixel, pixel, pixel, pixel );
}


void OptWidget::setMargins( int left, int top, int right, int bottom )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setContentsMargins( left, top, right, bottom );
}


QWidget *OptWidget::firstWidget( void ) const
{
  return FirstWidget;
}


QWidget *OptWidget::lastWidget( void ) const
{
  return LastWidget;
}


int OptWidget::selectMask( void ) const
{
  return SelectMask;
}


int OptWidget::readOnlyMask( void ) const
{
  return ReadOnlyMask;
}


bool OptWidget::continuousUpdate( void ) const
{
  return ContinuousUpdate;
}


int OptWidget::changedFlag( void )
{
  return ChangedFlag;
}


Options *OptWidget::options( void ) const
{
  return Opt;
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


void OptWidget::disableUpdate( void )
{
  DisableUpdate = true;
}


void OptWidget::enableUpdate( void )
{
  DisableUpdate = false;
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
  // set color roles:
  QPalette::ColorRole bcr = base ? QPalette::Base : QPalette::Window;
  QPalette::ColorRole fcr = base ? QPalette::Text : QPalette::WindowText;

  // get background and foreground colors:
  QColor bg( w->palette().color( QPalette::Active, bcr ) );
  QColor fg( w->palette().color( QPalette::Active, fcr ) );

  // set foreground color:
  bool fgchanged = false;  
  if ( ( style & LabelBlue ) == LabelBlue ) {
    fg = Qt::blue;
    fgchanged = true;
  }
  else if ( style & LabelRed ) {
    fg = Qt::red;
    fgchanged = true;
  }
  else if ( style & LabelGreen ) {
    fg = Qt::green;
    fgchanged = true;
  }
  
  // set background color:
  bool bgchanged = false;  
  if ( style & LabelBackBlack ) {
    w->setAutoFillBackground( true );
    bg = Qt::black;
    bgchanged = true;
  }
  else if ( style & LabelBackWhite ) {
    w->setAutoFillBackground( true );
    bg = Qt::white;
    bgchanged = true;
  }

  // set new palette:  
  if ( palette ) {
    QColor dc( Qt::blue );
    if ( fgchanged ) {
      QPalette qp( fg, dc, fg.lighter( 170 ), fg.darker( 200 ), fg.darker( 150 ), dc, dc, dc, bg );
      w->setPalette( qp );
    }
    else if ( bgchanged ) {
      QPalette qp( w->palette() );
      qp.setColor( bcr, bg );
      w->setPalette( qp );
    }
  }
  else {
    QPalette qp( w->palette() );
    if ( button ) {
      qp.setColor( QPalette::Button, bg );
      qp.setColor( QPalette::ButtonText, fg );
    }
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
	if ( Widgets[k]->param().ident() == ue->ident() ) {
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
	if ( Widgets[k]->param().flags( UpdateFlag ) ) {
	  Widgets[k]->reset();
	  Widgets[k]->param().delFlags( UpdateFlag );
	}
      }
    }
    else if ( e->type() == QEvent::User+4 ) {
      // updaeSettings( ident )
      UpdateIdentEvent *ue = (UpdateIdentEvent*)e;
      for ( unsigned int k=0; k<Widgets.size(); k++ ) {
	if ( Widgets[k]->param().ident() == ue->ident() ) {
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
	if ( Widgets[k]->param().flags( UpdateFlag ) ) {
	  Widgets[k]->update();
	  Widgets[k]->param().delFlags( UpdateFlag );
	}
      }
    }
    if ( OMutex != 0 )
      OMutex->unlock();
  }
}
   

}; /* namespace relacs */

#include "moc_optwidget.cc"

