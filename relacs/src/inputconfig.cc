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
    Layout=new QHBoxLayout(this);
    Buttonlayout=new QVBoxLayout();
    List<<"inputtraceid"<<"inputsamplerate"<<"inputtraceunit"<<"inputtracedevice"<<"inputtracechannel"<<"inputtracereference"<<"inputtracegain";
    
    Layout->addLayout(Buttonlayout);

    Widget=new QTableWidget(this);
    
    AddButton=new QPushButton("Add",this);
    RemoveButton=new QPushButton("Remove",this);
    AcceptButton=new QPushButton("Accept",this);
    
    Widget->setRowCount(1);
    Widget->setColumnCount(7);
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
  
  InputConfig::~InputConfig( void )
  {
  }
  

  void InputConfig::fillCells(int row, bool selection)
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
	      QComboBox *cb = new QComboBox();
	      cb->setEditable(true);
	      cb->addItem("mV");
	      cb->addItem("V");
	      cb->setCurrentIndex( ((QComboBox*)w)->currentIndex() );
	      Widget->setCellWidget(row, c, cb);
	    }
	    else if(w->inherits("QSpinBox")){
	      QSpinBox *gainBox = new QSpinBox();
	      gainBox->setRange(0,10);
	      gainBox->setValue( ((QSpinBox*)w)->value() );
	      Widget->setCellWidget(row, c, gainBox);
	    }
	    else if( w->inherits("QDoubleSpinBox")) {
	      QDoubleSpinBox *samplerateBox = new QDoubleSpinBox();
	      samplerateBox->setRange(1.0,100.0);
	      samplerateBox->setValue( ((QDoubleSpinBox*)w)->value() );
	      samplerateBox->setSuffix(" kHz");
	      Widget->setCellWidget(row, c, samplerateBox);
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

      Widget->setItem(row, 0, new QTableWidgetItem(tr("V-%1").arg(row+1)));
      Widget->setCellWidget(row, 1, samplerateBox);
      Widget->setCellWidget(row, 2, cb);
      Widget->setItem(row, 3, new QTableWidgetItem(QString("0")));
      Widget->setItem(row, 4, new QTableWidgetItem(tr("%1").arg(row)));
      Widget->setItem(row, 5, new QTableWidgetItem(QString("ground")));
      Widget->setCellWidget(row, 6, gainBox);
    }
  }


  void InputConfig::addRow()
  {
    bool ok;
    int n = QInputDialog::getInteger(this, tr("QInputDialog::getInteger()"), tr("Add # rows:"), 1, 0, 100, 1, &ok);

    if( ok )
    {
      for(int i=0; i<n; i++) {
	int row = Widget->rowCount();

	//QItemSelectionModel * Selection = Widget->selectionModel();
	//QModelIndexList Indexes = Selection->selectedRows();

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

  void InputConfig::deleteRow()
  {
    //QItemSelectionModel * Selection = Widget->selectionModel();
    //QModelIndexList Indexes = Selection->selectedRows();


    /*foreach( const QModelIndex & index, Indexes ) {
      if (index.isValid()) {
	cout << index.row() << endl; 
	Widget->removeRow(index.row());
      }
      }*/
 

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

  void InputConfig::accept()
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


#include "moc_inputconfig.cc"
