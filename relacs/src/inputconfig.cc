/*
  inputconfig.cc
  Configures analog input traces.

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

#include <cmath>
#include <cctype>
#include <iostream>
#include <QStringList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QApplication>
#include <relacs/indata.h>
#include <relacs/doublespinbox.h>
#include <relacs/inputconfig.h>

namespace relacs {


InputConfig::InputConfig( Options &opts, QWidget *parent )
  : QWidget( parent ),
    Opts( opts ),
    Processing( false )
{
  // the table for editing the traces:  
  Table = new QTableWidget( this );
  Table->setSelectionMode( QAbstractItemView::ContiguousSelection );
  fillTable();
  // Table->resizeColumnsToContents();
  int w = Table->verticalHeader()->width() + Table->frameWidth()*2 + 40;
  for ( int i = 0; i < Table->columnCount(); i++ )
    w += Table->columnWidth( i ); 
  Table->setMinimumWidth( w );
  Table->setMinimumHeight( 16*Table->rowHeight( 0 ) );
  Table->horizontalHeader()->setResizeMode( QHeaderView::Stretch ); 

  // layout:
  QHBoxLayout *layout = new QHBoxLayout;

  layout->addWidget( Table );
  
  QVBoxLayout *buttonlayout = new QVBoxLayout;
  layout->addLayout( buttonlayout );
  QPushButton *insertbutton = new QPushButton( "&Insert", this );
  buttonlayout->addWidget( insertbutton );
  QObject::connect( insertbutton, SIGNAL( clicked() ), this, SLOT( insertRows() ) );
  QPushButton *erasebutton = new QPushButton( "&Erase", this );
  buttonlayout->addWidget( erasebutton );
  QObject::connect( erasebutton, SIGNAL( clicked() ), this, SLOT( eraseRows() ) );
  QPushButton *fillbutton = new QPushButton( "&Fill", this );
  buttonlayout->addWidget( fillbutton );
  QObject::connect( fillbutton, SIGNAL( clicked() ), this, SLOT( fillCells() ) );

  setLayout( layout );
}
 
 
void InputConfig::fillTable( void )
{
  Table->setColumnCount( 9 );
  QStringList list;
  list << "Name" << "Device" << "Channel" << "Reference" << "Sampling rate" << "Max. value" << "Scale" << "Unit" << "Center";
  Table->setHorizontalHeaderLabels( list );

  int nid = Opts.size( "inputtraceid" );
  Table->setRowCount( nid );
  for ( int k=0; k<nid; k++ ) {
    // read out settings:
    string traceid = Opts.text( "inputtraceid", k, "" );
    string device = Opts.text( "inputtracedevice", k, "" );
    int channel = Opts.integer( "inputtracechannel", k, 0 );
    string reference = Opts.text( "inputtracereference", k, "ground" );
    double samplerate = Opts.number( "inputsamplerate", 1000.0 );
    double maxvalue = Opts.number( "inputtracemaxvalue", k, 1.0 );
    double scale = Opts.number( "inputtracescale", k, 1.0 );
    if ( fabs( scale ) < 1e-8 )
      scale = 1.0;
    string unit = Opts.text( "inputtraceunit", k, "V" );
    bool center = Opts.boolean( "inputtracecenter", k, false );

    // add settings to table:
    fillRow( k, traceid, device, channel, reference, samplerate, maxvalue, scale, unit, center );
  }
}


void InputConfig::fillRow( int row, const string &name, const string &device, int channel,
			   const string &reference, double samplerate, double maxvalue,
			   double scale, const string &unit, bool center )
{
  Table->setItem( row, 0, new QTableWidgetItem( name.c_str() ) );
  Table->setItem( row, 1, new QTableWidgetItem( device.c_str() ) );
  QSpinBox *channelbox = new QSpinBox;
  channelbox->setRange( 0, 1024 );
  channelbox->setValue( channel );
  Table->setCellWidget( row, 2, channelbox );
  QComboBox *referencebox = new QComboBox;
  referencebox->setEditable( false );
  referencebox->addItem( InData::referenceStr( InData::RefDifferential ).c_str() );
  referencebox->addItem( InData::referenceStr( InData::RefCommon ).c_str() );
  referencebox->addItem( InData::referenceStr( InData::RefGround ).c_str() );
  referencebox->addItem( InData::referenceStr( InData::RefOther ).c_str() );
  for ( int c=0; c < referencebox->count(); c++ ) {
    if ( referencebox->itemText( c ).toStdString() == reference ) {
      referencebox->setCurrentIndex( c );
      break;
    }
  }
  Table->setCellWidget( row, 3, referencebox );
  DoubleSpinBox *sampleratebox = new DoubleSpinBox;
  sampleratebox->setFormat( "%g" );
  sampleratebox->setRange( 1.0, 1000.0 );
  sampleratebox->setValue( 0.001*samplerate );
  sampleratebox->setSuffix( " kHz" );
  Table->setCellWidget( row, 4, sampleratebox );
  DoubleSpinBox *maxvalbox = new DoubleSpinBox;
  maxvalbox->setFormat( "%g" );
  maxvalbox->setRange( 0.0, 1000000.0 );
  maxvalbox->setValue( maxvalue );
  Table->setCellWidget( row, 5, maxvalbox );
  DoubleSpinBox *scalebox = new DoubleSpinBox;
  scalebox->setFormat( "%g" );
  scalebox->setRange( 0.0, 1000000.0 );
  scalebox->setValue( scale );
  Table->setCellWidget( row, 6, scalebox );
  QComboBox *unitbox = new QComboBox;
  unitbox->setEditable( true );
  unitbox->addItem( unit.c_str() );
  unitbox->addItem( "V" );
  unitbox->addItem( "mV" );
  unitbox->addItem( "uV" );
  unitbox->addItem( "kV" );
  unitbox->addItem( "A" );
  unitbox->addItem( "mA" );
  unitbox->addItem( "uA" );
  unitbox->addItem( "nA" );
  unitbox->addItem( "pA" );
  unitbox->addItem( "kA" );
  Table->setCellWidget( row, 7, unitbox );
  QCheckBox *centerbox = new QCheckBox;
  centerbox->setChecked( center );
  Table->setCellWidget( row, 8, centerbox );
}


void InputConfig::getRow( int row, string &basename, int &nameinx, string &device, int &channel,
			  string &reference, double &samplerate, double &maxvalue,
			  double &scale, string &unit, bool &center )
{
  basename = Table->item( row, 0 )->text().toStdString();
  int k=0;
  for ( k=(int)basename.length()-1; k>=0 && isdigit( basename[k] ); k-- );
  nameinx = (int)::round( Str( basename.substr( k+1 ) ).number( 0.0 ) );
  basename.erase( k+1 );
  if ( basename.empty() )
    basename = "V-";
  else if ( basename[basename.size()-1] != '-' )
    basename.push_back( '-' );
  device = Table->item( row, 1 )->text().toStdString();
  QSpinBox *cb = (QSpinBox *)Table->cellWidget( row, 2 );
  channel = cb->value();
  QComboBox *rb = (QComboBox *)Table->cellWidget( row, 3 );
  reference = rb->currentText().toStdString();
  DoubleSpinBox *srb = (DoubleSpinBox *)Table->cellWidget( row, 4 );
  samplerate = 1000.0*srb->value();
  DoubleSpinBox *mb = (DoubleSpinBox *)Table->cellWidget( row, 5 );
  maxvalue = mb->value();
  DoubleSpinBox *sb = (DoubleSpinBox *)Table->cellWidget( row, 6 );
  scale = sb->value();
  QComboBox *ub = (QComboBox *)Table->cellWidget( row, 7 );
  unit = ub->currentText().toStdString();
  QCheckBox *cvb = (QCheckBox *)Table->cellWidget( row, 8 );
  center = cvb->isChecked();
}


void InputConfig::insertRows( void )
{
  if ( Processing )
    return;
  Processing = true;

  bool ok;
  int n = QInputDialog::getInteger( this, "Input trace configuration", "Add # rows:", 1, 0, 1024, 1, &ok );
  if ( ! ok ) {
    Processing = false;
    return;
  }

  QList<QTableWidgetSelectionRange> selection = Table->selectedRanges();
  int row = 0;
  if ( selection.empty() || selection.front().columnCount() < 9 ) {
    // no row is selected, append new rows:
    row = Table->rowCount();
  }
  else {
    // a row is selected, insert rows before that row:
    row = selection.front().bottomRow()+1;
  }

  // fill in the rows:
  string name = "V-";
  int nameinx = 1;
  string device = "ai-1";
  int channel = 0;
  string reference = InData::referenceStr( InData::RefGround );
  double samplerate = 20000.0;
  double maxvalue = 1.0;
  double scale = 1.0;
  string unit = "V";
  bool center = true;
  if ( row > 0 ) {
    getRow( row-1, name, nameinx, device, channel,
	    reference, samplerate, maxvalue, scale, unit, center );
    if ( nameinx == 0 ) {
      nameinx = 1;
      /*
      // rename non numbered names to a numbered name:
      // check for name doublets:
      for ( int j=0; j<Table->rowCount(); j++ ) {
	string rowname = Table->item( j, 0 )->text().toStdString();
	if ( name+Str( nameinx ) == rowname ) {
	  int k=0;
	  for ( k=(int)rowname.length()-1; k>=0 && isdigit( rowname[k] ); k-- );
	  nameinx = (int)::round( Str( rowname.substr( k+1 ) ).number( 0.0 ) ) + 1;
	  j=0;
	}
      }
      Table->item( row-1, 0 )->setText( string( name+Str( nameinx ) ).c_str() );
      */
    }
    else
      nameinx++;
    channel++;
  }
  for ( int i=0; i<n; i++ ) {
    // check for name doublets:
    for ( int j=0; j<Table->rowCount(); j++ ) {
      string rowname = Table->item( j, 0 )->text().toStdString();
      if ( name+Str( nameinx ) == rowname ) {
	int k=0;
	for ( k=(int)rowname.length()-1; k>=0 && isdigit( rowname[k] ); k-- );
	nameinx = (int)::round( Str( rowname.substr( k+1 ) ).number( 0.0 ) ) + 1;
	j=0;
      }
    }
    // check for channel doublets:
    for ( int j=0; j<Table->rowCount(); j++ ) {
      string rowdevice = Table->item( j, 1 )->text().toStdString();
      QSpinBox *cb = (QSpinBox *)Table->cellWidget( j, 2 );
      int rowchannel = cb->value();
      if ( device == rowdevice && channel == rowchannel ) {
	channel = rowchannel+1;
	j=0;
      }
    }
    // insert row:
    Table->insertRow( row+i );
    fillRow( row+i, name+Str( nameinx ), device, channel, reference, samplerate, maxvalue, scale, unit, center );
    nameinx++;
    channel++;
  }

  Processing = false;
}


void InputConfig::eraseRows( void )
{
  if ( Processing )
    return;
  Processing = true;

  QList<QTableWidgetSelectionRange> selection = Table->selectedRanges();
  if ( selection.empty() || selection.front().columnCount() < 9 ) {
    Processing = false;
    return;
  }

  for ( int i=0; i<selection.front().rowCount(); i++ )
    Table->removeRow( selection.front().topRow() );

  Processing = false;
}


void InputConfig::fillCells( void )
{
  if ( Processing )
    return;
  Processing = true;

  QList<QTableWidgetSelectionRange> selection = Table->selectedRanges();
  if ( selection.empty() || selection.front().rowCount() <= 1 || selection.front().columnCount() < 1 ){
    Processing = false;
    return;
  }

  string name = "V-";
  int nameinx = 1;
  string device = "ai=1";
  int channel = 0;
  string reference = InData::referenceStr( InData::RefGround );
  double samplerate = 20000.0;
  double maxvalue = 1.0;
  double scale = 1.0;
  string unit = "V";
  bool center = true;
  getRow( selection.front().topRow(), name, nameinx, device, channel,
	  reference, samplerate, maxvalue, scale, unit, center );
  if ( nameinx == 0 ) {
    nameinx = 1;
    // check for name doublets:
    for ( int j=0; j<Table->rowCount(); j++ ) {
      string rowname = Table->item( j, 0 )->text().toStdString();
      if ( name+Str( nameinx ) == rowname ) {
	int k=0;
	for ( k=(int)rowname.length()-1; k>=0 && isdigit( rowname[k] ); k-- );
	nameinx = (int)::round( Str( rowname.substr( k+1 ) ).number( 0.0 ) ) + 1;
	j=0;
      }
    }
    Table->item( selection.front().topRow(), 0 )->setText( string( name+Str( nameinx ) ).c_str() );
  }
  else
    nameinx++;
  channel++;
  string prevdevice = device;

  for ( int row=selection.front().topRow()+1; row<=selection.front().bottomRow(); row++ ) {
    // name:
    if ( selection.front().leftColumn() <= 0 && selection.front().rightColumn() >= 0 ) {
      // check for name doublets:
      for ( int j=0; j<Table->rowCount(); j++ ) {
	if ( j < row || j > selection.front().bottomRow() ) {
	  string rowname = Table->item( j, 0 )->text().toStdString();
	  if ( name+Str( nameinx ) == rowname ) {
	    int k=0;
	    for ( k=(int)rowname.length()-1; k>=0 && isdigit( rowname[k] ); k-- );
	    nameinx = (int)::round( Str( rowname.substr( k+1 ) ).number( 0.0 ) ) + 1;
	    j=0;
	  }
	}
      }
      Table->item( row, 0 )->setText( string( name+Str( nameinx ) ).c_str() );
    }
    // device:
    if ( selection.front().leftColumn() <= 1 && selection.front().rightColumn() >= 1 )
      Table->item( row, 1 )->setText( device.c_str() );
    string rowdevice = Table->item( row, 1 )->text().toStdString();
    // channel:
    if ( rowdevice != prevdevice )
      channel = 0;
    if ( selection.front().leftColumn() <= 2 && selection.front().rightColumn() >= 2 ) {
      // check for channel doublets:
      for ( int j=0; j<Table->rowCount(); j++ ) {
	if ( j < row || j > selection.front().bottomRow() ) {
	  string rdevice = Table->item( j, 1 )->text().toStdString();
	  QSpinBox *cb = (QSpinBox *)Table->cellWidget( j, 2 );
	  int rchannel = cb->value();
	  if ( rowdevice == rdevice && channel == rchannel ) {
	    channel = rchannel+1;
	    j=0;
	  }
	}
      }
      QSpinBox *cb = (QSpinBox *)Table->cellWidget( row, 2 );
      cb->setValue( channel );
    }
    // reference:
    if ( selection.front().leftColumn() <= 3 && selection.front().rightColumn() >= 3 ) {
      QComboBox *rb = (QComboBox *)Table->cellWidget( row, 3 );
      for ( int c=0; c < rb->count(); c++ ) {
	if ( rb->itemText( c ).toStdString() == reference ) {
	  rb->setCurrentIndex( c );
	  break;
	}
      }
    }
    // sample rate:
    if ( selection.front().leftColumn() <= 4 && selection.front().rightColumn() >= 4 ) {
      DoubleSpinBox *srb = (DoubleSpinBox *)Table->cellWidget( row, 4 );
      srb->setValue( 0.001*samplerate );
    }
    // max value:
    if ( selection.front().leftColumn() <= 5 && selection.front().rightColumn() >= 5 ) {
      DoubleSpinBox *mb = (DoubleSpinBox *)Table->cellWidget( row, 5 );
      mb->setValue( maxvalue );
    }
    // scale:
    if ( selection.front().leftColumn() <= 6 && selection.front().rightColumn() >= 6 ) {
      DoubleSpinBox *sb = (DoubleSpinBox *)Table->cellWidget( row, 6 );
      sb->setValue( scale );
    }
    // unit:
    if ( selection.front().leftColumn() <= 7 && selection.front().rightColumn() >= 7 ) {
      QComboBox *ub = (QComboBox *)Table->cellWidget( row, 7 );
      bool found = false;
      for ( int c=0; c < ub->count(); c++ ) {
	if ( ub->itemText( c ).toStdString() == unit ) {
	  ub->setCurrentIndex( c );
	  found= true;
	  break;
	}
      }
      if ( ! found ) {
	ub->insertItem( 0, unit.c_str() );
	ub->setCurrentIndex( 0 );
      }
    }
    // center:
    if ( selection.front().leftColumn() <= 8 && selection.front().rightColumn() >= 8 ) {
      QCheckBox *cvb = (QCheckBox *)Table->cellWidget( row, 8 );
      cvb->setChecked( center );
    }

    nameinx++;
    channel++;
    prevdevice = rowdevice;
  }

  Processing = false;
}


void InputConfig::dialogClosed( int rv )
{
  if ( Processing )
    return;
  Processing = true;

  if ( rv < 1 || Table->rowCount() == 0 ) {
    Processing = false;
    if ( rv != 1 )
      delete this;
    return;
  }

  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

  // trace name:
  QTableWidgetItem *item = Table->item( 0, 0 );
  Parameter &np = Opts.setText( "inputtraceid", item->text().toStdString() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    item = Table->item( r, 0 );
    np.addText( item->text().toStdString() );
  }

  // device name:
  item = Table->item( 0, 1 );
  Parameter &dp = Opts.setText( "inputtracedevice", item->text().toStdString() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    item = Table->item( r, 1 );
    dp.addText( item->text().toStdString() );
  }

  // channel:
  QSpinBox *cb = (QSpinBox *)Table->cellWidget( 0, 2 );
  Parameter &cp = Opts.setInteger( "inputtracechannel", cb->value() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    cb = (QSpinBox *)Table->cellWidget( r, 2 );
    cp.addInteger( cb->value() );
  }

  // reference:
  QComboBox *rb = (QComboBox *)Table->cellWidget( 0, 3 );
  Parameter &rp = Opts.setText( "inputtracereference", rb->currentText().toStdString() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    rb = (QComboBox *)Table->cellWidget( r, 3 );
    rp.addText( rb->currentText().toStdString() );
  }

  // sampling rate:
  DoubleSpinBox *srb = (DoubleSpinBox *)Table->cellWidget( 0, 4 );
  Opts.setNumber( "inputsamplerate", 1000.0*srb->value() );

  // max value:
  DoubleSpinBox *mb = (DoubleSpinBox *)Table->cellWidget( 0, 5 );
  Parameter &mp = Opts.setNumber( "inputtracemaxvalue", mb->value() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    mb = (DoubleSpinBox *)Table->cellWidget( r, 5 );
    mp.addInteger( mb->value() );
  }

  // scale:
  DoubleSpinBox *sb = (DoubleSpinBox *)Table->cellWidget( 0, 6 );
  Parameter &sp = Opts.setNumber( "inputtracescale", sb->value() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    sb = (DoubleSpinBox *)Table->cellWidget( r, 6 );
    sp.addNumber( sb->value() );
  }

  // unit:
  QComboBox *ub = (QComboBox *)Table->cellWidget( 0, 7 );
  Parameter &up = Opts.setText( "inputtraceunit", ub->currentText().toStdString() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    ub = (QComboBox *)Table->cellWidget( r, 7 );
    up.addText( ub->currentText().toStdString() );
  }

  // center:
  QCheckBox *cvb = (QCheckBox *)Table->cellWidget( 0, 8 );
  Parameter &cvp = Opts.setBoolean( "inputtracecenter", cvb->isChecked() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    cvb = (QCheckBox *)Table->cellWidget( r, 8 );
    cvp.addInteger( cvb->isChecked() );
  }

  emit newInputSettings();

  QApplication::restoreOverrideCursor();

  Processing = false;

  if ( rv != 1 )
    delete this;
}


}; /* namespace relacs */


#include "moc_inputconfig.cc"
