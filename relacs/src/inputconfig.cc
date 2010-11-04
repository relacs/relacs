#include <relacs/inputconfig.h>
#include <QtGui>
#include <QTableWidget>
#include <QStringList>
#include <QPushButton>
#include <iostream>
using namespace std;

namespace relacs {

  InputConfig::InputConfig(QWidget *parent)
    : QWidget(parent)
  {
    layout=new QHBoxLayout(this);
    buttonlayout=new QVBoxLayout();
    list<<"inputtraceid"<<"inputsamplerate"<<"inputtraceunit"<<"inputtracedevice"<<"inputtracechannel"<<"inputtracereference"<<"inputtracegain";
    
    layout->addLayout(buttonlayout);

    widget=new QTableWidget(this);
    
    addButton=new QPushButton("Add",this);
    removeButton=new QPushButton("Remove",this);
    acceptButton=new QPushButton("Accept",this);
    
    widget->setRowCount(1);
    widget->setColumnCount(7);
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
  
  InputConfig::~InputConfig( void )
  {
  }
  

  void InputConfig::fillCells(int row, bool selection)
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
	      QComboBox *cb = new QComboBox();
	      cb->setEditable(true);
	      cb->addItem("mV");
	      cb->addItem("V");
	      cb->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
	      widget->setCellWidget(row, c, cb);
	    }
	    else if(w->inherits("QSpinBox")){
	      QSpinBox *gainBox = new QSpinBox();
	      gainBox->setRange(0,10);
	      gainBox->setValue( ((QSpinBox*)w)->value() );
	      widget->setCellWidget(row, c, gainBox);
	    }
	    else if( w->inherits("QDoubleSpinBox")) {
	      QDoubleSpinBox *samplerateBox = new QDoubleSpinBox();
	      samplerateBox->setRange(1.0,100.0);
	      samplerateBox->setValue( ((QDoubleSpinBox*)w)->value() );
	      samplerateBox->setSuffix(" kHz");
	      widget->setCellWidget(row, c, samplerateBox);
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

      widget->setItem(row, 0, new QTableWidgetItem(tr("V-%1").arg(row+1)));
      widget->setCellWidget(row, 1, samplerateBox);
      widget->setCellWidget(row, 2, cb);
      widget->setItem(row, 3, new QTableWidgetItem(QString("0")));
      widget->setItem(row, 4, new QTableWidgetItem(tr("%1").arg(row)));
      widget->setItem(row, 5, new QTableWidgetItem(QString("ground")));
      widget->setCellWidget(row, 6, gainBox);
    }
  }


  void InputConfig::addRow()
  {
    bool ok;
    int n = QInputDialog::getInteger(this, tr("QInputDialog::getInteger()"), tr("Add # rows:"), 1, 0, 100, 1, &ok);

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

  void InputConfig::deleteRow()
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

  void InputConfig::accept()
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


#include "moc_inputconfig.cc"
