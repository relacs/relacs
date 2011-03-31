#include <relacs/outputconfig.h>
#include <QtGui>
#include <QTableWidget>
#include <QStringList>
#include <QPushButton>
#include <iostream>
using namespace std;

namespace relacs {

  OutputConfig::OutputConfig(QWidget *parent)
    : QWidget(parent)
  {
    Layout=new QHBoxLayout(this);
    Buttonlayout=new QVBoxLayout();
    List<<"outputtraceid"<<"outputtracemaxrate"<<"outputtraceunit"<<"outputtracedevice"<<"outputtracechannel"<<"outputtracescale"<<"outputtracedelay"<<"outputtracemodality";
    
    Layout->addLayout(Buttonlayout);

    Widget=new QTableWidget(this);
    
    AddButton=new QPushButton("Add",this);
    RemoveButton=new QPushButton("Remove",this);
    AcceptButton=new QPushButton("Accept",this);
    
    Widget->setRowCount(1);
    Widget->setColumnCount(8);
    Widget->setHorizontalHeaderLabels(List);
       
    Layout->addWidget(Widget);
    Buttonlayout->addWidget(AddButton);
    Buttonlayout->addWidget(RemoveButton);
    Buttonlayout->addWidget(AcceptButton);
    
    setLayout(Layout);

    Selection = Widget->selectionModel();
    Indexes = Selection->selectedRows();

    fillCells(0, false);

    Widget->resizeColumnsToContents();
    Widget->resize(1000, 600);
    
    QObject::connect(AddButton, SIGNAL(clicked()), this, SLOT(addRow()));
    QObject::connect(RemoveButton, SIGNAL(clicked()), this, SLOT(deleteRow()));
    QObject::connect(AcceptButton, SIGNAL(clicked()), this, SLOT(accept()));
  }
  
  OutputConfig::~OutputConfig( void )
  {
  }
  

  void OutputConfig::fillCells(int row, bool selection)
  {
    if(selection){

      for(int c=0; c<Widget->columnCount(); c++) {

	QTableWidgetItem *item = Widget->item(row-1,c);
	if(item){

	  QString t = item->text();
	  QStringList list1 = t.split("-");

	  if(list1.size()>1){
	    int v = list1[list1.size()-1].toInt();
	    Widget->setItem(row, c, new QTableWidgetItem(list1[0].append(tr(+"-%1").arg(v+1))));
	  }
	  else if( t.data()->isNumber() ){
	    int v = t.toInt();
	    Widget->setItem(row, c, new QTableWidgetItem(tr("%1").arg(v+1)));
	  }
	  else
	    Widget->setItem(row, c, item);
	}
	else {
	  QWidget* w =  Widget->cellWidget(row-1,c);
	  
	  if(w) {
	    if(w->inherits("QComboBox")){
	      if(Widget->horizontalHeaderItem(c)->text().toStdString()=="outputtraceunit") {
		QComboBox *cb = new QComboBox();
		cb->setEditable(true);
		cb->addItem("mV");
		cb->addItem("V");
		cb->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
		Widget->setCellWidget(row, c, cb);
	      }
	      else if(Widget->horizontalHeaderItem(c)->text().toStdString()=="outputtraceid") {
		QComboBox *cbSpeaker = new QComboBox();
		cbSpeaker->setEditable(true);
		cbSpeaker->addItem("Left-Speaker");
		cbSpeaker->addItem("Right-Speaker");
		cbSpeaker->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
		Widget->setCellWidget(row, c, cbSpeaker);
	      }
	    }
	    else if( w->inherits("QDoubleSpinBox")) {
	      if(Widget->horizontalHeaderItem(c)->text().toStdString()=="outputtracemaxrate") {
		QDoubleSpinBox *rateBox = new QDoubleSpinBox();
		rateBox->setRange(1.0,500.0);
		rateBox->setValue( ((QDoubleSpinBox*)w)->value() );
		rateBox->setSuffix(" kHz");
		Widget->setCellWidget(row, c, rateBox);
	      }
	      else if(Widget->horizontalHeaderItem(c)->text().toStdString()=="outputtracedelay") {
		QDoubleSpinBox *delayBox = new QDoubleSpinBox();
		delayBox->setRange(0.0,100.0);
		delayBox->setValue( ((QDoubleSpinBox*)w)->value() );
		delayBox->setSuffix(" ms");
		Widget->setCellWidget(row, c, delayBox);
	      }
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

      QComboBox *cbSpeaker = new QComboBox();
      cbSpeaker->setEditable(true);
      cbSpeaker->addItem("Left-Speaker");
      cbSpeaker->addItem("Right-Speaker");
      
      QDoubleSpinBox *rateBox = new QDoubleSpinBox();
      rateBox->setRange(1.0,500.0);
      rateBox->setValue(240.0);
      rateBox->setSuffix(" kHz");

      QDoubleSpinBox *delayBox = new QDoubleSpinBox();
      delayBox->setRange(0.0,100.0);
      delayBox->setValue(0.0);
      delayBox->setSuffix(" ms");

      Widget->setCellWidget(row, 0, cbSpeaker);
      Widget->setCellWidget(row, 1, rateBox);
      Widget->setCellWidget(row, 2, cb);
      Widget->setItem(row, 3, new QTableWidgetItem(tr("ao-%1").arg(row+1)));
      Widget->setItem(row, 4, new QTableWidgetItem(tr("%1").arg(row)));
      Widget->setItem(row, 5, new QTableWidgetItem(QString("1")));
      Widget->setCellWidget(row, 6, delayBox);
      Widget->setItem(row, 7, new QTableWidgetItem(QString("voltage")));
    }
  }


  void OutputConfig::addRow()
  {
    bool ok;
    int n = QInputDialog::getInteger(this, tr("QOutputDialog::getInteger()"), tr("Add # rows:"), 1, 0, 100, 1, &ok);

    if( ok )
    {
      for(int i=0; i<n; i++) {
	int row = Widget->rowCount();

	//QItemSelectionModel * Selection = Widget->selectionModel();
	//QModelIndexList Indexes = Selection->selectedRows();
	Indexes = Selection->selectedRows();

	if(Indexes.count()>0) {
	  for(int j=0; j<Indexes.count(); j++) {
	    QModelIndex index = Indexes.at(j);
	    row = index.row()+i+Indexes.count();
	    Widget->insertRow(row);
	    fillCells(row, true);
	  }
	}
	else {
	   Widget->insertRow(row);
	   fillCells(row, false);
	}

	Selection->clear();
	Selection->clearSelection();
	Selection->reset();
      }
    }
  }

  void OutputConfig::deleteRow()
  {
    //QItemSelectionModel * Selection = Widget->selectionModel();
    //QModelIndexList Indexes = Selection->selectedRows();


    /*foreach( const QModelIndex & index, Indexes ) {
      if (index.isValid()) {
	cout << index.row() << endl; 
	Widget->removeRow(index.row());
      }
      }*/

    
    
    Indexes = Selection->selectedRows();

    int j=0;

    for(int i=0; i<Indexes.count(); i++) {
      QModelIndex index = Indexes.at(i);
      int r = index.row();
      cout << r-j << "\t" << Widget->rowCount() << endl;
      if(r-j>=0 && r-j<Widget->rowCount() && index.isValid() ) {
	Widget->removeRow(r-j);
	j++;
      }
    }

    

    Selection->clear();
    Selection->clearSelection();
    Selection->reset();

  }

  void OutputConfig::accept()
  {
    for(int c=0; c<Widget->columnCount(); c++) {
      cout << Widget->horizontalHeaderItem(c)->text().toStdString() << "\t";
    }
    cout << endl;

    for(int r=0; r<Widget->rowCount(); r++) {
      for(int c=0; c<Widget->columnCount(); c++) {
	//	cout << Widget->horizontalHeaderItem(c)->text().toStdString() << ": " << endl;
	QTableWidgetItem *item =  Widget->item(r,c);
	if(item)
	  cout << item->text().toStdString() << "\t";
	else {
	  QWidget* w =  Widget->cellWidget(r,c);
	  if(w->inherits("QComboBox"))
	    cout << ((QComboBox*)w)->itemText(((QComboBox*)w)->currentIndex()).toStdString() << "\t";
	  else if(w->inherits("QSpinBox"))
	    cout << ((QSpinBox*)w)->value() << ((QSpinBox*)w)->suffix().toStdString() << "\t";
	   else if( w->inherits("QDoubleSpinBox") )
	     cout << ((QDoubleSpinBox*)w)->value() << ((QDoubleSpinBox*)w)->suffix().toStdString() << "\t";
	}
      }
      cout << "\n";
    }
  }

}; /* namespace relacs */


#include "moc_outputconfig.cc"
