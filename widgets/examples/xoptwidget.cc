/*
  xoptwidget.cc
  

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

#include <iostream>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <relacs/optdialog.h>
#include "mainwidget.h"
using namespace relacs;


MainWidget::MainWidget( QWidget *parent, const char *name )
  : QVBox( parent, name )
{
  Opt1.addLabel( "Timing", 0, OptWidget::Bold );
  Opt1.addNumber( "duration", "Duration of Signal",
		  0.3, 0.01, 1.0, 0.000001, "seconds", "ms" ).setStyle( OptWidget::Huge + OptWidget::Italic + OptWidget::Green );
  Opt1.addNumber( "pause", "Pause between Signals",
		 0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", 3 ).setStyle( OptWidget::LabelNormal + OptWidget::ValueItalic + OptWidget::LabelBold +  OptWidget::ValueGreen + OptWidget::ValueBackBlack + OptWidget::ValueLCD );
  Opt1.setNumber( "pause", 0.180 );
  Opt1.addInteger( "repeats", "Repeats", 8, 0, 100 );
  Opt1.addLabel( "Settings", 0,  OptWidget::Large + OptWidget::Bold +  OptWidget::Red );
  Opt1.addText( "color", "Color", "red|green|blue");
  Opt1.addText( "comment", "Comments", "no comment" );

  Opt2.addSeparator();
  Opt2.addBoolean( "sinewave", "Use Sine Wave", false );
  Opt2.addBoolean( "loop", "Loop", true, 1 ).setStyle( OptWidget::LabelSmall +  OptWidget::ValueRed );
  Opt2.addDate( "date", "Date", 2009, 6, 20 );
  Opt2.addTime( "time", "Time", 16, 42, 13 );

  QPushButton *dialogButton = new QPushButton( "&Dialog", this );
  QPushButton *quitButton = new QPushButton( "&Quit", this );
  connect( dialogButton, SIGNAL( clicked() ),
           this, SLOT( dialog() ) );
  connect( quitButton, SIGNAL( clicked() ),
           qApp, SLOT( quit() ) );
}


void MainWidget::dialog( void )
{
  QLabel *l = new QLabel( "Hello!", this );
  l->setAlignment( Qt::AlignCenter );
  l->setFont( QFont( "Helvetica", 24, 3 ) );
  OptDialog d( this );
  d.setCaption( "Example Dialog" );
  d.addWidget( l );
  d.addOptions( Opt1, 0, 1, 1 );
  d.addOptions( Opt2 );
  d.setSpacing( 4 );
  d.setMargin( 10 );
  d.addButton( "&Ok", OptDialog::Accept, 1 );
  d.addButton( "&Apply", OptDialog::Accept, 2, false );
  d.addButton( "&Reset", OptDialog::Reset );
  d.addButton( "&Close" );
  connect( &d, SIGNAL( dialogClosed( int ) ), this, SLOT( done( int ) ) );
  connect( &d, SIGNAL( buttonClicked( int ) ), this, SLOT( action( int ) ) );
  connect( &d, SIGNAL( valuesChanged( void ) ), this, SLOT( accepted( void ) ) );
  int r = d.exec();
  cerr << "dialog: " << r << endl << Opt1 << Opt2 << endl;
  Options co;
  co.assign( Opt1, 8 );
  cerr << "changed: " << endl << co << endl;
}


void MainWidget::done( int r )
{
  cerr << "dialog returned " << r << endl;
}


void MainWidget::action( int r )
{
  cerr << "dialog button pressed " << r << endl;
}


void MainWidget::accepted( void )
{
  cerr << "dialog accepted" << endl;
}


int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  MainWidget w;
  a.setMainWidget( &w );
  w.show();
  return a.exec();
}

#include "moc_mainwidget.cc"
