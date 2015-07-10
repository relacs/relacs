/*
  xoptwidget.cc
  

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

#include <iostream>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <relacs/optdialog.h>
#include "mainwidget.h"
using namespace relacs;


MainWidget::MainWidget( void )
  : QWidget()
{
  //  int teststyle = OptWidget::BackBlack | OptWidget::Red;
  int teststyle = OptWidget::ValueBackBlack | OptWidget::ValueGreen;
  int testflag = 0;
  Opt1.newSection( "Timing", 0, OptWidget::TabSection | teststyle );
  Opt1.addNumber( "duration", "Duration of Signal",
		  0.3, 0.01, 1.0, 0.000001, "seconds", "ms" ).setFlags( testflag ).setStyle( teststyle );
  // setStyle( OptWidget::Huge | OptWidget::Italic | OptWidget::Green | teststyle );
  Opt1.addNumber( "pause", "Pause between Signals",
		  0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", 3 ).setStyle( teststyle | OptWidget::ValueLCD );
  // .setStyle( OptWidget::ValueGreen | OptWidget::ValueBackBlack | OptWidget::ValueLCD | OptWidget::ValueHuge | teststyle );
  Opt1.addNumber( "delay", "Pause before signal",
		  0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", testflag ).setStyle( teststyle );
  // .setStyle( OptWidget::ValueGreen | OptWidget::ValueBackBlack | teststyle );
  Opt1.addInteger( "repeats", "Repeats", 8, 0, 100 ).setFlags( testflag ).setStyle( teststyle | OptWidget::SpecialInfinite );
  
  Opt1.addDate( "date", "Date", 2009, 6, 20, testflag ).setStyle( teststyle );
  Opt1.newSection( "Settings", 0,  teststyle );
  //  Opt1.newSection( "Settings", 0,  OptWidget::Large |  OptWidget::Red | teststyle );
  Opt1.addText( "fgcolor", "Foreground color", "red|green|blue", testflag ).setStyle( OptWidget::SelectText | teststyle );
  Opt1.addText( "bgcolor", "Background color", "red|green|blue", testflag ).setStyle( teststyle );
  Opt1.addText( "comment", "Comments", "no comment", testflag ).setStyle( teststyle );
  Opt1.addBoolean( "adjust", "Adjust input gain", true, testflag ).setStyle( teststyle );

  Opt1.newSection("Multiple", 0, OptWidget::TabSection | teststyle);
  Opt1.addText("multiple text", "allow multiple values", "test 1").setStyle(OptWidget::MultipleSelection)
      .addText("test 4").addText("test 2").addText("test 3");
  Opt1.addNumber("multiple double", "allow multiple values", 0).setStyle(OptWidget::MultipleSelection).setUnit("A")
      .addNumber(1.1).addNumber(2.2).addNumber(3.3).addNumber(4.4);
  Opt1.addInteger("multiple integer", "allow multiple values", 1).setStyle(OptWidget::MultipleSelection).setUnit("V")
      .addInteger(1).addInteger(2).addInteger(3).addInteger(4);
  Opt1.addText("combo text", "ComboBox multiple test", "value 1").setStyle(OptWidget::MultipleSelection | Parameter::Select)
      //.assign("[ [ value 1, value 2 ], [ value 1, value 2, value 3 ] ]");
      .assign("[ ~, ~, {value2}, value~3 ]");
      //.assign("[ value 1, [ value 1, value 2, value 3 ] ]");
      //.addSelectOption("value 1").addSelectOption("value 2").addSelectOption("value 3").addText("value 2");

  Opt1.newSection( "Analysis" ).setStyle( OptWidget::TabSection );
  Opt1.addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 1.0, 0.0, 100.0, 0.01, "seconds", "ms" );
  Opt1.addNumber( "sigma1", "Standard deviation of rate smoothing kernel 1", 0.001, 0.0, 1.0, 0.0001, "seconds", "ms" );
  Opt1.addNumber( "sigma2", "Standard deviation of rate smoothing kernel 2", 0.005, 0.0, 1.0, 0.001, "seconds", "ms" );
  Opt1.addNumber( "sigma3", "Standard deviation of rate smoothing kernel 3", 0.005, 0.0, 1.0, 0.001, "seconds", "ms" );
  Opt1.addBoolean( "adjust", "Adjust input gain", true );
  Opt1.newSection( "Save stimuli" );
  Opt1.addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  Opt1.addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );
  Opt1.addSelection( "storelevel", "Save", "all|generated|noise|none" ).setUnit( "stimuli" );

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

  QComboBox *qcb = new QComboBox();
  qcb->addItem( "red" );
  qcb->addItem( "green" );

  QSpinBox *qsb = new QSpinBox();

  QVBoxLayout *l = new QVBoxLayout;
  l->addWidget( dialogButton );
  l->addWidget( quitButton );
  l->addWidget( qcb );
  l->addWidget( qsb );
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
  d.addOptions( Opt1, 0, 1, OptWidget::BreakLinesStyle + OptWidget::BoldSectionsStyle );
  d.addOptions( Opt2, 0, 0, OptWidget::BoldSectionsStyle );
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
  cerr << "dialog exec() returned " << r << '\n';
  cerr << "the values of the options are:\n";
  cerr << Opt1 << Opt2 << '\n';
}


void MainWidget::done( int r )
{
  cerr << "dialog called done() and is about to return " << r << '\n';
}


void MainWidget::action( int r )
{
  cerr << "dialog button pressed that activated button with " << r << '\n';
}


void MainWidget::accepted( void )
{
  cerr << "dialog called accepted()" << '\n';
  Options co1;
  co1.assign( Opt1, Parameter::changedFlag() );
  Options co2;
  co2.assign( Opt2, Parameter::changedFlag() );
  cerr << "the following options have been changed: " << '\n'
       << co1 << '\n' << co2 << '\n';
}


int main( int argc, char **argv )
{
  QApplication a( argc, argv );
  MainWidget w;
  w.show();
  return a.exec();
}

#include "moc_mainwidget.cc"
