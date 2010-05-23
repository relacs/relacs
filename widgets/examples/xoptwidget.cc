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
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <relacs/optdialog.h>
#include "mainwidget.h"
using namespace relacs;


MainWidget::MainWidget( void )
  : QWidget()
{
  //int testflag = OptWidget::BackBlack | OptWidget::Blue;
  int testflag = 0;
  Opt1.addLabel( "Timing", 0, OptWidget::Bold + OptWidget::TabLabel | testflag );
  Opt1.addNumber( "duration", "Duration of Signal",
		  0.3, 0.01, 1.0, 0.000001, "seconds", "ms" ).setStyle( OptWidget::Huge | OptWidget::Italic | OptWidget::Green | testflag );
  Opt1.addNumber( "pause", "Pause between Signals",
		  0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", 3 ).setStyle( OptWidget::ValueGreen | OptWidget::ValueBackBlack | OptWidget::ValueLCD | OptWidget::ValueHuge | testflag );
  Opt1.addInteger( "repeats", "Repeats", 8, 0, 100 ).setStyle( testflag );
  Opt1.addLabel( "Settings", 0,  OptWidget::Bold | testflag );
  Opt1.addInteger( "repeats", "Repeats", 8, 0, 100 ).setStyle( testflag );

  Opt1.addLabel( "Settings", 0,  OptWidget::Large | OptWidget::Bold |  OptWidget::Red | testflag );
  Opt1.addText( "fgcolor", "Foreground color", "red|green|blue" ).setStyle( OptWidget::SelectText | testflag );
  Opt1.addText( "bgcolor", "Background color", "red|green|blue" ).setStyle( testflag );
  Opt1.addText( "comment", "Comments", "no comment" ).setStyle( testflag );
  Opt1.addBoolean( "adjust", "Adjust input gain", true ).setStyle( testflag );

  Opt1.addLabel( "Analysis" ).setStyle( OptWidget::TabLabel );
  Opt1.addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 1.0, 0.0, 100.0, 0.01, "seconds", "ms" );
  Opt1.addNumber( "sigma1", "Standard deviation of rate smoothing kernel 1", 0.001, 0.0, 1.0, 0.0001, "seconds", "ms" );
  Opt1.addNumber( "sigma2", "Standard deviation of rate smoothing kernel 2", 0.005, 0.0, 1.0, 0.001, "seconds", "ms" );
  Opt1.addNumber( "sigma3", "Standard deviation of rate smoothing kernel 3", 0.005, 0.0, 1.0, 0.001, "seconds", "ms" );
  Opt1.addBoolean( "adjust", "Adjust input gain", true );
  Opt1.addLabel( "Save stimuli" ).setStyle( OptWidget::Bold );
  Opt1.addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  Opt1.addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );
  Opt1.addSelection( "storelevel", "Save", "all|generated|noise|none" ).setUnit( "stimuli" );

  Opt2.addSeparator();
  Opt2.addBoolean( "sinewave", "Use Sine Wave", false );
  Opt2.addBoolean( "loop", "Loop", true, 1 ).setStyle( OptWidget::LabelSmall |  OptWidget::ValueRed );
  Opt2.addDate( "date", "Date", 2009, 6, 20 );
  Opt2.addTime( "time", "Time", 16, 42, 13 );

  QPushButton *dialogButton = new QPushButton( "&Dialog" );
  QPushButton *quitButton = new QPushButton( "&Quit" );
  connect( dialogButton, SIGNAL( clicked() ),
           this, SLOT( dialog() ) );
  connect( quitButton, SIGNAL( clicked() ),
           qApp, SLOT( quit() ) );

  QVBoxLayout *l = new QVBoxLayout;
  l->addWidget( dialogButton );
  l->addWidget( quitButton );
  setLayout( l );
}


void MainWidget::dialog( void )
{
  QLabel *l = new QLabel( "Hello!", this );
  l->setAlignment( Qt::AlignCenter );
  l->setFont( QFont( "Helvetica", 24, 3 ) );
  OptDialog d( this );
  d.setCaption( "Example Dialog" );
  d.addWidget( l );
  d.addOptions( Opt1, 0, 1, OptWidget::BreakLinesStyle );
  d.addOptions( Opt2 );
  d.setVerticalSpacing( 4 );
  d.setMargins( 10 );
  d.addButton( "&Ok", OptDialog::Accept, 1 );
  d.addButton( "&Apply", OptDialog::Accept, 2, false );
  d.addButton( "&Reset", OptDialog::Reset );
  d.addButton( "&Close" );
  connect( &d, SIGNAL( dialogClosed( int ) ), this, SLOT( done( int ) ) );
  connect( &d, SIGNAL( buttonClicked( int ) ), this, SLOT( action( int ) ) );
  connect( &d, SIGNAL( valuesChanged( void ) ), this, SLOT( accepted( void ) ) );
  int r = d.exec();
  cerr << "dialog: " << r << endl;
  cerr << Opt1 << Opt2 << endl;
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
  Options co1;
  co1.assign( Opt1, Parameter::changedFlag() );
  Options co2;
  co2.assign( Opt2, Parameter::changedFlag() );
  cerr << "changed: " << endl << co1 << endl << co2 << endl;
}


int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  MainWidget w;
  w.show();
  return a.exec();
}

#include "moc_mainwidget.cc"
