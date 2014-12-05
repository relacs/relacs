/*
  inputconfig.cc
  Configures analog input traces.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <iostream>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QHeaderView>
#include <relacs/indata.h>
#include <relacs/inputconfig.h>

namespace relacs {


InputConfig::InputConfig( Options &opts, QWidget *parent )
  : QWidget( parent ),
    Opts( opts )
{
  // the table for editing the traces:  
  Table = new QTableWidget( this );
  fillTable();
  Selection = Table->selectionModel();
  Indexes = Selection->selectedRows();
  //  Table->resizeColumnsToContents();
  int w = Table->verticalHeader()->width() + Table->frameWidth()*2+40;
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
  /*
  AddButton = new QPushButton( "Add", this );
  buttonlayout->addWidget( AddButton );
  QObject::connect( AddButton, SIGNAL( clicked() ), this, SLOT( addRow() ) );
  RemoveButton = new QPushButton( "Remove", this );
  buttonlayout->addWidget( RemoveButton );
  QObject::connect( RemoveButton, SIGNAL( clicked() ), this, SLOT( deleteRow() ) );
  */

  setLayout( layout );
}
 
 
void InputConfig::fillTable( void )
{
  Table->setColumnCount( 9 );
  QStringList list;
  list << "Name" << "Device" << "Channel" << "Reference" << "Sampling rate" << "Scale" << "Unit" << "Gain index" << "Center";
  Table->setHorizontalHeaderLabels( list );

  int nid = Opts.size( "inputtraceid" );
  Table->setRowCount( nid );
  for ( int k=0; k<nid; k++ ) {
    // read out settings:
    string traceid = Opts.text( "inputtraceid", k, "" );
    double samplerate = Opts.number( "inputsamplerate", 1000.0 );
    double scale = Opts.number( "inputtracescale", k, 1.0 );
    if ( fabs( scale ) < 1e-8 )
      scale = 1.0;
    string unit = Opts.text( "inputtraceunit", k, "V" );
    int channel = Opts.integer( "inputtracechannel", k, 0 );
    string device = Opts.text( "inputtracedevice", k, "" );
    bool center = Opts.boolean( "inputtracecenter", k, false );
    string reference = Opts.text( "inputtracereference", k, "ground" );
    int gainindex = Opts.integer( "inputtracegain", k, -1 );
    if ( gainindex < 0 )
      gainindex = 0;

    // add settings to table:
    int row = k;
    Table->setItem( row, 0, new QTableWidgetItem( traceid.c_str() ) );
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
    QDoubleSpinBox *sampleratebox = new QDoubleSpinBox;
    sampleratebox->setRange( 1.0, 1000.0 );
    sampleratebox->setValue( 0.001*samplerate );
    sampleratebox->setSuffix( " kHz" );
    Table->setCellWidget( row, 4, sampleratebox );
    QDoubleSpinBox *scalebox = new QDoubleSpinBox;
    scalebox->setRange( 0.0, 1000000.0 );
    scalebox->setValue( scale );
    Table->setCellWidget( row, 5, scalebox );
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
    Table->setCellWidget( row, 6, unitbox );
    QSpinBox *gainbox = new QSpinBox;
    gainbox->setRange( 0, 20 );
    gainbox->setValue( gainindex );
    Table->setCellWidget( row, 7, gainbox );
    QCheckBox *centerbox = new QCheckBox;
    centerbox->setChecked( center );
    Table->setCellWidget( row, 8, centerbox );
  }
}


void InputConfig::fillCells(int row, bool selection)
{
  /*
  if(selection){

    for(int c=0; c<Table->columnCount(); c++) {

      QTableWidgetItem *item = Table->item(row-1,c);
      if(item){

	QString t = item->text();
	QStringList list1 = t.split("-");

	if(list1.size()>1){
	  int v = list1[list1.size()-1].toInt();
	  Table->setItem(row, c, new QTableWidgetItem(list1[0].append(tr(+"-%1").arg(v+1))));
	}
	else if( t.data()->isNumber() ){
	  int v = t.toInt();
	  Table->setItem(row, c, new QTableWidgetItem(tr("%1").arg(v+1)));
	}
	else
	  Table->setItem(row, c, item);
      }
      else {
	QWidget* w =  Table->cellWidget(row-1,c);
	  
	if(w) {
	  if(w->inherits("QComboBox")){
	    QComboBox *cb = new QComboBox();
	    cb->setEditable(true);
	    cb->addItem("mV");
	    cb->addItem("V");
	    cb->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
	    Table->setCellWidget(row, c, cb);
	  }
	  else if(w->inherits("QSpinBox")){
	    QSpinBox *gainBox = new QSpinBox();
	    gainBox->setRange(0,10);
	    gainBox->setValue( ((QSpinBox*)w)->value() );
	    Table->setCellWidget(row, c, gainBox);
	  }
	  else if( w->inherits("QDoubleSpinBox")) {
	    QDoubleSpinBox *samplerateBox = new QDoubleSpinBox();
	    samplerateBox->setRange(1.0,100.0);
	    samplerateBox->setValue( ((QDoubleSpinBox*)w)->value() );
	    samplerateBox->setSuffix(" kHz");
	    Table->setCellWidget(row, c, samplerateBox);
	  }
	}
      }
    }
  }
  else {

    QComboBox *cb = new QComboBox();
    cb->setEditable(true);
    cb->addItem("mV");
    cb->addItem("V");
      
    QSpinBox *gainBox = new QSpinBox();
    gainBox->setRange(0,10);
    gainBox->setValue(4);
      
    QDoubleSpinBox *samplerateBox = new QDoubleSpinBox();
    samplerateBox->setRange(1.0,100.0);
    samplerateBox->setValue(20.0);
    samplerateBox->setSuffix(" kHz");

    Table->setItem(row, 0, new QTableWidgetItem(tr("V-%1").arg(row+1)));
    Table->setCellWidget(row, 1, samplerateBox);
    Table->setCellWidget(row, 2, cb);
    Table->setItem(row, 3, new QTableWidgetItem(QString("0")));
    Table->setItem(row, 4, new QTableWidgetItem(tr("%1").arg(row)));
    Table->setItem(row, 5, new QTableWidgetItem(QString("ground")));
    Table->setCellWidget(row, 6, gainBox);
  }
  */
}


void InputConfig::addRow()
{
  /*
  bool ok;
  int n = QInputDialog::getInteger(this, tr("QInputDialog::getInteger()"), tr("Add # rows:"), 1, 0, 100, 1, &ok);

  if( ok )
    {
      for(int i=0; i<n; i++) {
	int row = Table->rowCount();

	//QItemSelectionModel * Selection = Table->selectionModel();
	//QModelIndexList Indexes = Selection->selectedRows();

	if(Indexes.count()>0) {
	  for(int j=0; j<Indexes.count(); j++) {
	    QModelIndex index = Indexes.at(j);
	    row = index.row()+i+Indexes.count();
	    Table->insertRow(row);
	    fillCells(row, true);
	  }
	}
	else {
	  Table->insertRow(row);
	  fillCells(row, false);
	}

	Selection->clear();
	Selection->clearSelection();
	Selection->reset();
      }
    }
  */
}


void InputConfig::deleteRow()
{
  //QItemSelectionModel * Selection = Table->selectionModel();
  //QModelIndexList Indexes = Selection->selectedRows();


  /*foreach( const QModelIndex & index, Indexes ) {
    if (index.isValid()) {
    cout << index.row() << endl; 
    Table->removeRow(index.row());
    }
    }*/
 

  int j=0;

  for(int i=0; i<Indexes.count(); i++) {
    QModelIndex index = Indexes.at(i);
    int r = index.row();
    cout << r-j << "\t" << Table->rowCount() << endl;
    if(r-j>=0 && r-j<Table->rowCount() && index.isValid() ) {
      Table->removeRow(r-j);
      j++;
    }
  }

  Selection->clear();
  Selection->clearSelection();
  Selection->reset();

}


void InputConfig::dialogClosed( int rv )
{
  if ( rv < 1 || Table->rowCount() == 0 ) {
    if ( rv != 1 )
      delete this;
    return;
  }

  // sampling rate:  
  QDoubleSpinBox *srb = (QDoubleSpinBox *)Table->cellWidget( 0, 4 );
  Opts.setNumber( "inputsamplerate", 1000.0*srb->value() );

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

  // scale:
  QDoubleSpinBox *sb = (QDoubleSpinBox *)Table->cellWidget( 0, 5 );
  Parameter &sp = Opts.setNumber( "inputtracescale", sb->value() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    sb = (QDoubleSpinBox *)Table->cellWidget( r, 5 );
    sp.addNumber( sb->value() );
  }

  // unit:
  QComboBox *ub = (QComboBox *)Table->cellWidget( 0, 6 );
  Parameter &up = Opts.setText( "inputtraceunit", ub->currentText().toStdString() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    ub = (QComboBox *)Table->cellWidget( r, 6 );
    up.addText( ub->currentText().toStdString() );
  }

  // gain index:
  QSpinBox *gb = (QSpinBox *)Table->cellWidget( 0, 7 );
  Parameter &gp = Opts.setInteger( "inputtracegain", gb->value() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    gb = (QSpinBox *)Table->cellWidget( r, 7 );
    gp.addInteger( gb->value() );
  }

  // center:
  QCheckBox *cvb = (QCheckBox *)Table->cellWidget( 0, 8 );
  Parameter &cvp = Opts.setBoolean( "inputtracecenter", cvb->isChecked() );
  for ( int r=1; r<Table->rowCount(); r++ ) {
    cvb = (QCheckBox *)Table->cellWidget( r, 8 );
    cvp.addInteger( cvb->isChecked() );
  }

  emit newInputSettings();

  if ( rv != 1 )
    delete this;
}


}; /* namespace relacs */


#include "moc_inputconfig.cc"
