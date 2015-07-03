/*
  optwidget.cc
  A Widget for modifying Options.

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

#include <QCoreApplication>
#include <QVBoxLayout>
#include <QFileDialog>
#include <relacs/optwidget.h>
#include <relacs/optwidgetbase.h>

namespace relacs {


class UpdateEvent : public QEvent
{
public:
  UpdateEvent( int type )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Name( "" ), Flags( 0 ), Lock( true ) {};
  UpdateEvent( int type, const string &name )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Name( name ), Flags( 0 ), Lock( true ) {};
  UpdateEvent( int type, int flags )
    : QEvent( QEvent::Type( QEvent::User+type ) ), Name( "" ), Flags( flags ), Lock( true ) {};
  UpdateEvent( const UpdateEvent &ue )
    : QEvent( QEvent::Type( ue.type() ) ), Name( ue.Name ), Flags( ue.Flags ), Lock( true ) {};
  string name( void ) const { return Name; };
  int flags( void ) const { return Flags; };
  bool lock( void ) const { return Lock; };
  void setLock( bool lock ) { Lock = lock; };
private:
  string Name;
  int Flags;
  bool Lock;
};


OptWidget::OptWidget( QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( 0 ),
    Tabs( false ),
    MaxLines( 0 ),
    LeftMargin( -1 ),
    RightMargin( -1 ),
    TopMargin( -1 ),
    BottomMargin( -1 ),
    VerticalSpacing( -1 ),
    HorizontalSpacing( -1 ),
    Style( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
  GUIThread = QThread::currentThread();
}


OptWidget::OptWidget( Options *o, QMutex *mutex, 
                      QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( mutex ),
    Tabs( false ),
    MaxLines( 0 ),
    LeftMargin( -1 ),
    RightMargin( -1 ),
    TopMargin( -1 ),
    BottomMargin( -1 ),
    VerticalSpacing( -1 ),
    HorizontalSpacing( -1 ),
    Style( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
  GUIThread = QThread::currentThread();
  assign( o );
}


OptWidget::OptWidget( Options *o, int selectmask, int romask,
                      bool contupdate, int style, QMutex *mutex,
                      string *tabhotkeys,
                      QWidget *parent, Qt::WindowFlags f )
  : QWidget( parent, f ),
    Opt( 0 ),
    MainWidget( 0 ),
    FirstWidget( 0 ),
    LastWidget( 0 ),
    OMutex( mutex ),
    Tabs( false ),
    MaxLines( 0 ),
    LeftMargin( -1 ),
    RightMargin( -1 ),
    TopMargin( -1 ),
    BottomMargin( -1 ),
    VerticalSpacing( -1 ),
    HorizontalSpacing( -1 ),
    Style( 0 ),
    Widgets(),
    DisableUpdate( false ),
    SelectMask( 0 ),
    ReadOnlyMask( 0 ),
    ContinuousUpdate( false )
{
  GUIThread = QThread::currentThread();
  assign( o, selectmask, romask, contupdate, style, mutex, tabhotkeys );
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


void OptWidget::assignOptions( Options *o, bool tabs, int &row, int &level,
                               QWidget *parent, QTabWidget *tabwidget,
                               string *tabhotkeys )
{
  level++;

  // parameter:
  Options::iterator pp = o->begin();
  // next selected parameter:
  for ( ; pp != o->end(); ++pp ) {
    if ( pp->flags( SelectMask ) )
      break;
  }
  QLabel *l;
  while ( pp != o->end() ) {
    bool added = true;
    // request string:
    string rs = pp->request();
    if ( Style & NameFrontStyle ) {
      if ( Style & HighlightNameStyle )
        rs = "<nobr><tt>" + pp->name() + "</tt>: " + rs + "</nobr>";
      else
        rs = pp->name() + ": " + rs;
    }
    else if ( Style & NameBehindStyle ) {
      if ( Style & HighlightNameStyle )
        rs = "<nobr>" + rs + " <tt>(" + pp->name() + ")</tt></nobr>";
      else
        rs += " (" + pp->name() + ")";
    }

    // NEW: multiple
    if ( (pp->style() & MultipleSelection) && (pp->isText() || pp->isAnyNumber()))
    {
      l = new QLabel( rs.c_str() );
      setLabelStyle( l, pp->style() );
      if ( Style & BreakLinesStyle )
        Layout.back()->addWidget( l, row, 1, 1, 2,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      else
        Layout.back()->addWidget( l, row, 1,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      QLabel *ul = 0;
      if ( ! pp->outUnit().empty() && ( pp->style() & Browse ) == 0 ) {
        ul = unitLabel( *pp );
        setLabelStyle( ul, pp->style() );
        if ( Style & BreakLinesStyle )
          Layout.back()->addWidget( ul, row+1, 2,
                                    Qt::AlignLeft | Qt::AlignVCenter );
        else
          Layout.back()->addWidget( ul, row, 3,
                                    Qt::AlignLeft | Qt::AlignVCenter );
      }
      OptWidgetMultipleValues* t = new OptWidgetMultipleValues(pp, l, Opt, this, OMutex);
      if ( t->editable() ) {
        if ( FirstWidget == 0 )
          FirstWidget = t->valueWidget();
        LastWidget = t->valueWidget();
      }
      if ( Style & BreakLinesStyle ) {
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
    else
    // text:
    if ( pp->isText() ) {
      l = new QLabel( rs.c_str() );
      setLabelStyle( l, pp->style() );
      if ( Style & BreakLinesStyle )
        Layout.back()->addWidget( l, row, 1, 1, 2,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      else
        Layout.back()->addWidget( l, row, 1,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      QLabel *ul = 0;
      if ( ! pp->outUnit().empty() && ( pp->style() & Browse ) == 0 ) {
        ul = unitLabel( *pp );
        setLabelStyle( ul, pp->style() );
        if ( Style & BreakLinesStyle )
          Layout.back()->addWidget( ul, row+1, 2,
                                    Qt::AlignLeft | Qt::AlignVCenter );
        else
          Layout.back()->addWidget( ul, row, 3,
                                    Qt::AlignLeft | Qt::AlignVCenter );
      }
      if ( pp->size() <= 1 ) {
        OptWidgetText *t = new OptWidgetText( pp, l, Opt, this, OMutex );
        if ( t->editable() ) {
          if ( FirstWidget == 0 )
            FirstWidget = t->valueWidget();
          LastWidget = t->valueWidget();
        }
        if ( Style & BreakLinesStyle ) {
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
          if ( Style & BreakLinesStyle )
            Layout.back()->addWidget( t->browseButton(), row, 2,
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
        if ( Style & BreakLinesStyle ) {
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
    else if ( pp->isNumber() || pp->isInteger() ) {
      l = new QLabel( rs.c_str() );
      setLabelStyle( l, pp->style() );
      if ( Style & BreakLinesStyle )
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
      if ( Style & BreakLinesStyle ) {
        row++;
        Layout.back()->addWidget( n->valueWidget(), row, 1 );
      }
      else
        Layout.back()->addWidget( n->valueWidget(), row, 2 );
      QLabel *ul = unitLabel( *pp );
      setLabelStyle( ul, pp->style() );
      if ( Style & BreakLinesStyle )
        Layout.back()->addWidget( ul, row, 2,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      else
        Layout.back()->addWidget( ul, row, 3,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      n->setUnitLabel( ul );
    }
    // boolean:
    else if ( pp->isBoolean() ) {
      OptWidgetBoolean *b = new OptWidgetBoolean( pp, Opt, this, rs, OMutex );
      if ( b->editable() ) {
        if ( FirstWidget == 0 )
          FirstWidget = b->valueWidget();
        LastWidget = b->valueWidget();
      }
      if ( Style & BreakLinesStyle )
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
    else if ( pp->isDate() ) {
      l = new QLabel( rs.c_str() );
      setLabelStyle( l, pp->style() );
      if ( Style & BreakLinesStyle )
        Layout.back()->addWidget( l, row, 1, 1, 2,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      else
        Layout.back()->addWidget( l, row, 1,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      OptWidgetDate *d = new OptWidgetDate( pp, l, Opt, this, OMutex );
      if ( d->editable() ) {
        if ( FirstWidget == 0 )
          FirstWidget = d->valueWidget();
        LastWidget = d->valueWidget();
      }
      if ( Style & BreakLinesStyle ) {
        row++;
        Layout.back()->addWidget( d->valueWidget(), row, 1 );
      }
      else
        Layout.back()->addWidget( d->valueWidget(), row, 2 );
    }
    // time:
    else if ( pp->isTime() ) {
      l = new QLabel( rs.c_str() );
      setLabelStyle( l, pp->style() );
      if ( Style & BreakLinesStyle )
        Layout.back()->addWidget( l, row, 1, 1, 2,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      else
        Layout.back()->addWidget( l, row, 1,
                                  Qt::AlignLeft | Qt::AlignVCenter );
      OptWidgetTime *t = new OptWidgetTime( pp, l, Opt, this, OMutex );
      if ( t->editable() ) {
        if ( FirstWidget == 0 )
          FirstWidget = t->valueWidget();
        LastWidget = t->valueWidget();
      }
      if ( Style & BreakLinesStyle ) {
        row++;
        Layout.back()->addWidget( t->valueWidget(), row, 1 );
      }
      else
        Layout.back()->addWidget( t->valueWidget(), row, 2 );
    }
    // nothing:
    else {
      added = false;
      row--;
    }
    // extra space:
    if ( (Style & ExtraSpaceStyle) > 0 && added ) {
      row++;
      Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum,
                                               QSizePolicy::Expanding ),
                              row, 2 );
    }
    row++;
    // max line count:
    if ( row > MaxLines ) {
      MaxLines = row;
    }
    // next selected parameter:
    for ( ++pp; pp != o->end(); ++pp ) {
      if ( pp->flags( SelectMask ) )
        break;
    }
  }

  // sections:
  bool firstsec = true;
  for ( Options::section_iterator sp = o->sectionsBegin();
        sp != o->sectionsEnd();
        ++sp ) {
    if ( (*sp)->size( SelectMask ) <= 0 )
      continue;
    if ( tabs && level == 0 &&
         (*sp)->flag( SelectMask ) && (*sp)->size( SelectMask ) > 0 &&
         ( (Style & TabSectionStyle ) ||
           ( (*sp)->style() & TabSection ) ||
           firstsec ) ) {
      // finish parameter:
      if ( row > 0 && (Style & ExtraSpaceStyle) == 0 )
        Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum,
                                                 QSizePolicy::Expanding ),
                                row, 2 );
      // this section starts a new tab:
      QWidget *w = new QWidget;
      tabwidget->addTab( w, tabLabel( (*sp)->name(), tabhotkeys ).c_str() );
      parent = w;
      row = 0;
      // add layout to current widget:
      Layout.push_back( new QGridLayout );
      if ( VerticalSpacing >= 0 )
        Layout.back()->setVerticalSpacing( VerticalSpacing );
      if ( HorizontalSpacing >= 0 )
        Layout.back()->setHorizontalSpacing( HorizontalSpacing );
      if ( TopMargin >= 0 )
        Layout.back()->setContentsMargins( LeftMargin, TopMargin,
                                           RightMargin, BottomMargin );
      parent->setLayout( Layout.back() );
    }
    else if ( (*sp)->flag( SelectMask ) ) {
      // this section is set as a label:
      OptWidgetSection *s = new OptWidgetSection( sp, Opt, this, OMutex );
      int sstyle = 0;
      if ( (Style & BoldSectionsStyle ) )
        sstyle |= Bold;
      setLabelStyle( s->valueWidget(), (*sp)->style() | sstyle );
      if ( Style & BreakLinesStyle ) {
        Layout.back()->addWidget( s->valueWidget(), row, 0, 1, 4,
                                  Qt::AlignLeft | Qt::AlignBottom );
      }
      else {
        Layout.back()->addWidget( s->valueWidget(), row, 0, 1, 5,
                                  Qt::AlignLeft | Qt::AlignBottom );
        // the following is necessary to eliminate expanding space on top of the label:
        QLabel *spacer = new QLabel;
        spacer->setFixedHeight( s->valueWidget()->sizeHint().height() );
        Layout.back()->addWidget( spacer, row, 2 );
      }
      Layout.back()->setColumnMinimumWidth( 0, 20 );
      row++;
    }
    assignOptions( *sp, tabs, row, level, parent, tabwidget, tabhotkeys );
    firstsec = false;
  }

  level--;
}


OptWidget &OptWidget::assign( Options *o, int selectmask, int romask,
                              bool contupdate, int style, QMutex *mutex,
                              string *tabhotkeys )
{
  Style = style;

  if ( mutex != 0 )
    OMutex = mutex;

  string mytabhotkeys = "";
  if ( tabhotkeys == 0 )
    tabhotkeys = &mytabhotkeys;

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

  lockMutex();

  if ( Opt->empty() ) {
    unlockMutex();
    return *this;
  }

  SelectMask = selectmask;
  ReadOnlyMask = romask;
  ContinuousUpdate = contupdate;

  MainWidget = this;
  QWidget *parent = this;
  QTabWidget *tabwidget = 0;

  // check for tab style:
  Tabs = false;
  for ( Options::const_section_iterator sp = o->sectionsBegin();
        sp != o->sectionsEnd();
        ++sp ) {
    if ( (*sp)->flag( SelectMask ) && (*sp)->size( SelectMask ) > 0 ) {
      Tabs = ( (style & TabSectionStyle ) || ( (*sp)->style() & TabSection ) );
      if ( Tabs )
        break;
    }
  }

  QVBoxLayout *l = new QVBoxLayout;
  l->setSpacing( 0 );
  l->setContentsMargins( 0, 0, 0, 0 );
  parent->setLayout( l );
  bool needgridlayout = false;
  if ( Tabs ) {
    tabwidget = new QTabWidget;
    l->addWidget( tabwidget );
    MainWidget = tabwidget;
    // is there a parameter?
    for ( Options::iterator pp = o->begin(); pp != o->end(); ++pp ) {
      if ( pp->flags( SelectMask ) ) {
        // top level Options has parameter, we need to add a tab:
        QWidget *w = new QWidget;
        tabwidget->addTab( w, tabLabel( Opt->name(), tabhotkeys ).c_str() );
        parent = w;
        needgridlayout = true;
        break;
      }
    }
  }
  else {
    QWidget *w = new QWidget;
    l->addWidget( w );
    MainWidget = w;
    parent = w;
    needgridlayout = true;
  }

  // add layout to current widget:
  if ( needgridlayout ) {
    Layout.push_back( new QGridLayout );
    if ( VerticalSpacing >= 0 )
      Layout.back()->setVerticalSpacing( VerticalSpacing );
    if ( HorizontalSpacing >= 0 )
      Layout.back()->setHorizontalSpacing( HorizontalSpacing );
    if ( TopMargin >= 0 )
      Layout.back()->setContentsMargins( LeftMargin, TopMargin,
                                         RightMargin, BottomMargin );
    parent->setLayout( Layout.back() );
  }

  int row = 0;
  int level = -1;
  assignOptions( Opt, Tabs, row, level, parent, tabwidget, tabhotkeys );

  // finish parameter:
  if ( row > 0 && (style & ExtraSpaceStyle) == 0 )
    Layout.back()->addItem( new QSpacerItem( 10, 0, QSizePolicy::Minimum,
                                             QSizePolicy::Expanding ),
                            row, 2 );

  // init activation/inactivation:
  for ( unsigned int k=0; k<Widgets.size(); k++ ) {
    if ( Widgets[k]->param() == Opt->end() )
      continue;
    for ( int j=0; j<Widgets[k]->param()->activations(); j++ ) {
      string aname = Widgets[k]->param()->activationName( j );
      if ( aname.size() > 0 ) {
        for ( unsigned int i=0; i<Widgets.size(); i++ ) {
          if ( Widgets[i]->param() == Opt->end() )
            continue;
          if ( *Widgets[i]->param() == aname )
            Widgets[i]->addActivation( j, Widgets[k] );
        }
      }
    }
  }

  unlockMutex();

  MainWidget->show();

  return *this;
}


string OptWidget::tabLabel( string label, string *tabhotkeys )
{
  size_t i = label.find( '&' );
  if ( i != string::npos && i+1<label.size() )
    label.erase( i, 1 );
  else
    i=0;
  size_t ti = Str( label ).low().find_first_not_of( *tabhotkeys, i );
  if ( ti != string::npos ) {
    *tabhotkeys += Str( label[ti] ).low();
    label.insert( ti, "&" );
  }
  return label;
}


bool OptWidget::tabs( void ) const
{
  return Tabs;
}


int OptWidget::lines( void ) const
{
  return MaxLines;
}


void OptWidget::setVerticalSpacing( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setVerticalSpacing( pixel );
  VerticalSpacing = pixel;
}


void OptWidget::setHorizontalSpacing( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setHorizontalSpacing( pixel );
  HorizontalSpacing = pixel;
}


void OptWidget::setMargins( int pixel )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setContentsMargins( pixel, pixel, pixel, pixel );
  LeftMargin = pixel;
  RightMargin = pixel;
  TopMargin = pixel;
  BottomMargin = pixel;
}


void OptWidget::setMargins( int left, int top, int right, int bottom )
{
  for ( unsigned int k=0; k<Layout.size(); k++ )
    Layout[k]->setContentsMargins( left, top, right, bottom );
  LeftMargin = left;
  RightMargin = right;
  TopMargin = top;
  BottomMargin = bottom;
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


int OptWidget::style( void ) const
{
  return Style;
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


void OptWidget::lockMutex( void )
{
  if ( OMutex != 0 )
    OMutex->lock();
}


bool OptWidget::tryLockMutex( int timeout )
{
  if ( OMutex != 0 )
    return OMutex->tryLock( timeout );
  else
    return true;
}


void OptWidget::unlockMutex( void )
{
  if ( OMutex != 0 )
    OMutex->unlock();
}


void OptWidget::addWidget( OptWidgetBase *owb )
{
  if ( owb != 0 )
    Widgets.push_back( owb );
}


bool OptWidget::updateDisabled( void ) const
{
  return DisableUpdate;
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


void OptWidget::setLabelColorStyle( QWidget *w, long style, RoleType rt )
{
  // set color roles:
  QPalette::ColorRole bcr = QPalette::Window;
  QPalette::ColorRole fcr = QPalette::WindowText;
  if ( rt == Text || rt == TextShade ) {
    bcr = QPalette::Base;
    fcr = QPalette::Text;
  }
  else if ( rt == Button ) {
    bcr = QPalette::Button;
    fcr = QPalette::ButtonText;
  }
  else if ( rt == Combo ) {
    bcr = QPalette::Button;
    fcr = QPalette::Text;
  }

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

  if ( rt == TextShade ) {
    // set new palette:
    QPalette qp( fg, bg, fg.lighter( 170 ), fg.darker( 200 ), fg.darker( 150 ),
                 fg, Qt::white, fg, bg );
    w->setPalette( qp );
  }
  else {
    // set specific colors:
    QPalette qp( w->palette() );
    if ( fgchanged )
      qp.setColor( fcr, fg );
    if ( bgchanged )
      qp.setColor( bcr, bg );
    w->setPalette( qp );
  }
}


void OptWidget::setLabelStyle( QWidget *w, long style, RoleType rt )
{
  setLabelFontStyle( w, style );
  setLabelColorStyle( w, style, rt );
}


void OptWidget::setValueStyle( QWidget *w, long style, RoleType rt )
{
  setLabelStyle( w, style >> 12, rt );
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
  lockMutex();

  // get the values:
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->get();

  // notify:
  DisableUpdate = true;
  Opt->callNotifies();
  DisableUpdate = false;

  // react to accept:
  emit valuesChanged();

  // clear changedflag:
  if ( clearchanged )
    Opt->delFlags( changedFlag() );

  unlockMutex();
}


void OptWidget::accept( void )
{
  accept( true );
}


void OptWidget::acceptGet( void )
{
  lockMutex();

  // get the values:
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->get();

  unlockMutex();
}


void OptWidget::acceptNotify( bool clearchanged )
{
  lockMutex();

  // notify:
  DisableUpdate = true;
  Opt->callNotifies();
  DisableUpdate = false;

  // react to accept:
  emit valuesChanged();

  // clear changedflag:
  if ( clearchanged )
    Opt->delFlags( changedFlag() );

  unlockMutex();
}


void OptWidget::reset( void )
{
  lockMutex();
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->reset();
  unlockMutex();
}


void OptWidget::resetDefault( void )
{
  lockMutex();
  for ( unsigned int k=0; k<Widgets.size(); k++ )
    Widgets[k]->resetDefault();
  unlockMutex();
}


void OptWidget::updateValue( const string &name )
{
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 1, name ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 1, name );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::updateValues( void )
{
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 2 ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 2 );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::updateValues( int flag, bool delflags )
{
  if ( Opt == 0 )
    return;
  // save flag:
  Opt->addFlags( UpdateFlag, flag );
  if ( delflags )
    Opt->delFlags( flag );
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 3, UpdateFlag ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 3, UpdateFlag );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::updateSettings( const string &name )
{
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 4, name ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 4, name );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::updateSettings( void )
{
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 5 ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 5 );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::updateSettings( int flag, bool delflags )
{
  if ( Opt == 0 )
    return;
  Opt->addFlags( UpdateFlag, flag );
  if ( delflags )
    Opt->delFlags( flag );
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new UpdateEvent( 6, UpdateFlag ) );
  else {
    UpdateEvent *ue = new UpdateEvent( 6, UpdateFlag );
    ue->setLock( false );
    customEvent( ue );
    delete ue;
  }
}


void OptWidget::customEvent( QEvent *e )
{
  if ( e->type() >= QEvent::User+1 && e->type() <= QEvent::User+6 ) {
    UpdateEvent *ue = dynamic_cast<UpdateEvent*>( e );
    if ( ue->lock() && ! tryLockMutex( 2 ) ) {
      // we do not get the lock for the data now,
      // so we repost the event to a later time.
      QCoreApplication::postEvent( this, new UpdateEvent( *ue ) );
      return;
    }
    switch ( e->type() - QEvent::User ) {
      case 1: {
        // updateValues( name )
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          if ( Widgets[k]->param() != Opt->end() &&
               Widgets[k]->param()->name() == ue->name() ) {
            Widgets[k]->reset();
            break;
          }
        }
        break;
      }
      case 2: {
        // updateValues()
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          Widgets[k]->reset();
        }
        break;
      }
      case 3: {
        // updateValues( flags )
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          if ( Widgets[k]->param() != Opt->end() &&
               Widgets[k]->param()->flags( ue->flags() ) ) {
            Widgets[k]->reset();
            Widgets[k]->param()->delFlags( ue->flags() );
          }
        }
        break;
      }
      case 4: {
        // updateSettings( name )
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          if ( Widgets[k]->param() != Opt->end() &&
               Widgets[k]->param()->name() == ue->name() ) {
            Widgets[k]->update();
            break;
          }
        }
        break;
      }
      case 5: {
        // updateSetings()
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          Widgets[k]->update();
        }
        break;
      }
      case 6: {
        // updateSettings( flags )
        for ( unsigned int k=0; k<Widgets.size(); k++ ) {
          if ( Widgets[k]->param() != Opt->end() &&
               Widgets[k]->param()->flags( ue->flags() ) ) {
            Widgets[k]->update();
            Widgets[k]->param()->delFlags( ue->flags() );
          }
        }
        break;
      }
    }
    if ( ue->lock() )
      unlockMutex();
  }
  else
    QWidget::customEvent( e );
}


}; /* namespace relacs */

#include "moc_optwidget.cc"

