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
    layout=new QHBoxLayout(this);
    buttonlayout=new QVBoxLayout();
    list<<"outputtraceid"<<"outputtracemaxrate"<<"outputtraceunit"<<"outputtracedevice"<<"outputtracechannel"<<"outputtracescale"<<"outputtracedelay"<<"outputtracemodality";
    
    layout->addLayout(buttonlayout);

    widget=new QTableWidget(this);
    
    addButton=new QPushButton("Add",this);
    removeButton=new QPushButton("Remove",this);
    acceptButton=new QPushButton("Accept",this);
    
    widget->setRowCount(1);
    widget->setColumnCount(8);
    widget->setHorizontalHeaderLabels(list);
       
    layout->addWidget(widget);
    buttonlayout->addWidget(addButton);
    buttonlayout->addWidget(removeButton);
    buttonlayout->addWidget(acceptButton);
    
    setLayout(layout);


    fillCells(0, false);

    widget->resizeColumnsToContents();
    widget->resize(1000, 600);
    
    QObject::connect(addButton, SIGNAL(clicked()), this, SLOT(addRow()));
    QObject::connect(removeButton, SIGNAL(clicked()), this, SLOT(deleteRow()));
    QObject::connect(acceptButton, SIGNAL(clicked()), this, SLOT(accept()));
  }
  
  OutputConfig::~OutputConfig( void )
  {
  }
  

  void OutputConfig::fillCells(int row, bool selection)
  {
    if(selection){

      for(int c=0; c<widget->columnCount(); c++) {

	QTableWidgetItem *item = widget->item(row-1,c);
	if(item){

	  QString t = item->text();
	  QStringList list1 = t.split("-");

	  if(list1.size()>1){
	    int v = list1[list1.size()-1].toInt();
	    widget->setItem(row, c, new QTableWidgetItem(list1[0].append(tr(+"-%1").arg(v+1))));
	  }
	  else if( t.data()->isNumber() ){
	    int v = t.toInt();
	    widget->setItem(row, c, new QTableWidgetItem(tr("%1").arg(v+1)));
	  }
	  else
	    widget->setItem(row, c, item);
	}
	else {
	  QWidget* w =  widget->cellWidget(row-1,c);
	  
	  if(w) {
	    if(w->inherits("QComboBox")){
	      if(widget->horizontalHeaderItem(c)->text().toStdString()=="outputtraceunit") {
		QComboBox *cb = new QComboBox();
		cb->setEditable(true);
		cb->addItem("mV");
		cb->addItem("V");
		cb->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
		widget->setCellWidget(row, c, cb);
	      }
	      else if(widget->horizontalHeaderItem(c)->text().toStdString()=="outputtraceid") {
		QComboBox *cbSpeaker = new QComboBox();
		cbSpeaker->setEditable(true);
		cbSpeaker->addItem("Left-Speaker");
		cbSpeaker->addItem("Right-Speaker");
		cbSpeaker->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
		widget->setCellWidget(row, c, cbSpeaker);
	      }
	    }
	    else if( w->inherits("QDoubleSpinBox")) {
	      if(widget->horizontalHeaderItem(c)->text().toStdString()=="outputtracemaxrate") {
		QDoubleSpinBox *rateBox = new QDoubleSpinBox();
		rateBox->setRange(1.0,500.0);
		rateBox->setValue( ((QDoubleSpinBox*)w)->value() );
		rateBox->setSuffix(" kHz");
		widget->setCellWidget(row, c, rateBox);
	      }
	      else if(widget->horizontalHeaderItem(c)->text().toStdString()=="outputtracedelay") {
		QDoubleSpinBox *delayBox = new QDoubleSpinBox();
		delayBox->setRange(0.0,100.0);
		delayBox->setValue( ((QDoubleSpinBox*)w)->value() );
		delayBox->setSuffix(" ms");
		widget->setCellWidget(row, c, delayBox);
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

      widget->setCellWidget(row, 0, cbSpeaker);
      widget->setCellWidget(row, 1, rateBox);
      widget->setCellWidget(row, 2, cb);
      widget->setItem(row, 3, new QTableWidgetItem(tr("ao-%1").arg(row+1)));
      widget->setItem(row, 4, new QTableWidgetItem(tr("%1").arg(row)));
      widget->setItem(row, 5, new QTableWidgetItem(QString("1")));
      widget->setCellWidget(row, 6, delayBox);
      widget->setItem(row, 7, new QTableWidgetItem(QString("voltage")));
    }
  }


  void OutputConfig::addRow()
  {
    bool ok;
    int n = QInputDialog::getInteger(this, tr("QOutputDialog::getInteger()"), tr("Add # rows:"), 1, 0, 100, 1, &ok);

    if( ok )
    {
      for(int i=0; i<n; i++) {
	int row = widget->rowCount();

	QItemSelectionModel * selection = widget->selectionModel();
	QModelIndexList indexes = selection->selectedRows(0);

	if(indexes.count()>0) {
	  for(int j=0; j<indexes.count(); j++) {
	    QModelIndex index = indexes.at(j);
	    row = index.row()+i+indexes.count();
	    widget->insertRow(row);
	    fillCells(row, true);
	  }
	}
	else {
	   widget->insertRow(row);
	   fillCells(row, false);
	}
      }
    }
  }

  void OutputConfig::deleteRow()
  {
    QItemSelectionModel * selection = widget->selectionModel();
    QModelIndexList indexes = selection->selectedRows(0);

    int j=0;

    for(int i=0; i<indexes.count(); i++) {
      QModelIndex index = indexes.at(i);
      int r = index.row();
      cout << r-j << "\t" << widget->rowCount() << endl;
      if(r-j>=0 && r-j<widget->rowCount() ) {
	widget->removeRow(r-j);
	j++;
      }
    }

  }

  void OutputConfig::accept()
  {
    for(int c=0; c<widget->columnCount(); c++) {
      cout << widget->horizontalHeaderItem(c)->text().toStdString() << "\t";
    }
    cout << endl;

    for(int r=0; r<widget->rowCount(); r++) {
      for(int c=0; c<widget->columnCount(); c++) {
	//	cout << widget->horizontalHeaderItem(c)->text().toStdString() << ": " << endl;
	QTableWidgetItem *item =  widget->item(r,c);
	if(item)
	  cout << item->text().toStdString() << "\t";
	else {
	  QWidget* w =  widget->cellWidget(r,c);
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
