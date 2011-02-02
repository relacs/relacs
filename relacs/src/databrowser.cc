/*
  databrowser.cc
  Interface for browsing previously recorded data.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <QtGui>
#include <QHBoxLayout>
#include <relacs/repro.h>
#include <relacs/databrowser.h>
#include <relacs/datafile.h>
#include <relacs/sampledata.h>
#include <relacs/tabledata.h>
#include <relacs/tablekey.h>
#include <relacs/options.h>
#include <relacs/repro.h>
#include <dirent.h>
#include <string>
#include <map>


namespace relacs {


struct DataBrowser::Stimulus
{
  string name;
  string swVersion;
  long index;
  double duration;
  double soundintensity;
  double vFactor;
  double time;
  double stepsize;
  Options Header;
  Options Option;

};

struct DataBrowser::Rep
{

  //Options Header;
  Options Option;

};

struct DataBrowser::Cell
{

  Options Head;

};


DataBrowser::DataBrowser( QWidget *parent )
  : QWidget( parent )
{
  TreeWidget = new QTreeWidget;
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget( TreeWidget );
  setLayout( layout );

  QStringList headerLabels;
  headerLabels << QObject::tr("Recordings");
  TreeWidget->setHeaderLabels(headerLabels);


  DataBrowser::Cells = new map< string, map<long,DataBrowser::Stimulus>* >;
  DataBrowser::Protocol = new map< string, map<int,DataBrowser::Rep>* >;
  Header = new map< string, DataBrowser::Cell* >;


  QObject::connect( TreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int )), this, SLOT(list(QTreeWidgetItem *, int)));
  QObject::connect( TreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int )), this, SLOT(list(QTreeWidgetItem *, int)));
  QObject::connect( TreeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int )), this, SLOT(list(QTreeWidgetItem *, int)));

  DataBrowser::session = false;
  DataBrowser::folder = "."; 
  load(folder);
}


DataBrowser::~DataBrowser( void )
{
}


void DataBrowser::addStimulus(const OutData &Signal)
{
  cout << "Stimulus: " << Signal.traceName() << endl;

  if ( TreeWidget->topLevelItemCount() > 0  && session==true )
    TreeWidget->currentItem()->addChild( new QTreeWidgetItem( (QTreeWidget*)0, QStringList( Signal.traceName().c_str() ) ) );
}


void DataBrowser::addStimulus(const OutList &Signal)
{
  for(int i=0; i<Signal.size(); i++) {
    OutData data = Signal[i];
    if ( TreeWidget->topLevelItemCount() > 0  && session==true )
      TreeWidget->currentItem()->addChild( new QTreeWidgetItem( (QTreeWidget*)0, QStringList( data.traceName().c_str() ) ) );
  }
}


void DataBrowser::addSession( const string &path )
{
  TreeWidget->addTopLevelItem( new QTreeWidgetItem( (QTreeWidget*)0,
						    QStringList( path.c_str() ) ) );
  DataBrowser::session = true;
}


void DataBrowser::endSession()
{
  QTreeWidgetItem * item = TreeWidget->topLevelItem(TreeWidget->topLevelItemCount()-1);

  QString s =  item->text(0); 
  cout << "end session: " << s.toStdString() << endl;

  int i = TreeWidget->indexOfTopLevelItem( item );
  TreeWidget->takeTopLevelItem(i);
  delete item;

  // TreeWidget->removeItemWidget( TreeWidget->topLevelItem( TreeWidget->topLevelItemCount()-1 ), 0 );

  DataBrowser::session = false;
}


void DataBrowser::addRepro( const RePro *Repro )
{
  if ( TreeWidget->topLevelItemCount() > 0 && session==true ) {
    QTreeWidgetItem * CurrentRepro = new QTreeWidgetItem( (QTreeWidget*)0, QStringList( Repro->name().c_str() ) );
    TreeWidget->topLevelItem( TreeWidget->topLevelItemCount()-1 )->addChild( CurrentRepro );
    TreeWidget->setCurrentItem(CurrentRepro);
    //TreeWidget->topLevelItem( TreeWidget->topLevelItemCount()-1 )->addChild( new QTreeWidgetItem( (QTreeWidget*)0, QStringList( Repro->name().c_str() ) ) );
  }
}


void DataBrowser::load( const string &dir )
{
  DIR *hdir;
  struct dirent *entry;

  hdir = opendir(dir.c_str());
  //DataBrowser::folder = dir;

  do
  {
    entry = readdir(hdir);
    if (entry)
    {
      if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0) {
	QTreeWidgetItem *parent = new QTreeWidgetItem(TreeWidget, QStringList(QObject::tr(entry->d_name)), -1);
       
	TreeWidget->insertTopLevelItem(0, parent);
	TreeWidget->setCurrentItem(parent);

	//DataBrowser::read(entry->d_name, parent);

      }
    }
  } while (entry);
  closedir(hdir);

  TreeWidget->sortItems(0,Qt::DescendingOrder);

}

void DataBrowser::list(QTreeWidgetItem * item, int col)
{
  // cout <<  treeWidget.currentColumn() << endl;

  int type = item->type();
  //cout << "Typ: " << type << endl;

  map<string, map<long,DataBrowser::Stimulus>* >::iterator iterCell;
  iterCell=DataBrowser::Cells->find(item->text(col).toStdString());


  if(type<0 && iterCell==DataBrowser::Cells->end()) {
    DataBrowser::read(item->text(0).toStdString(), item);
    item->setExpanded(true);
  }


  //bin2dat
  if(type>0) {

    QTreeWidgetItem *parent = item->parent(); 
    //cout << parent->indexOfChild(item) << ": plot!\n";

    long fnumber = type; //item->text(col).toLong();

    map<string, map<long,DataBrowser::Stimulus>* >::iterator iterCell;
    iterCell=DataBrowser::Cells->find(parent->parent()->text(col).toStdString());

    map<string, map<int,DataBrowser::Rep>* >::iterator iterRepro;
    iterRepro=DataBrowser::Protocol->find(parent->parent()->text(col).toStdString());

    if(iterCell!=DataBrowser::Cells->end() && iterRepro!=DataBrowser::Protocol->end()) {
      map<long,DataBrowser::Stimulus> * currentCell = iterCell->second;

      map<long,DataBrowser::Stimulus>::iterator it;
      it=currentCell->find(fnumber);

      map<int,DataBrowser::Rep> * currentCell2 = iterRepro->second;

      map<int,DataBrowser::Rep>::iterator itr;
      
      QTreeWidgetItem *head = item->parent(); 
      itr=currentCell2->find(head->parent()->indexOfChild(head));
      // cout << "repro: " << head->parent()->indexOfChild(head) << endl;
      
      if(it!=currentCell->end() && itr!=currentCell2->end()) {
	DataBrowser::Stimulus CurrentStimulus = it->second;
	DataBrowser::Rep CurrentRepro = itr->second;
	
	string filename = CurrentStimulus.name;
	/*double dur = CurrentStimulus.duration;
	double stepsize = CurrentStimulus.stepsize;*/

	Options Opt = CurrentRepro.Option;

	// Metadata options output
	//DataBrowser::metadata->assign(&opt, 0, 1);
		
	
	/*SampleDataD sdata(0.0, dur, stepsize);

	string s;
	if(CurrentStimulus.swVersion.compare("oel")==0) {
	  s = DataBrowser::folder + filename + "/traces.sw1"; //oel
	  const char* binfile =  s.c_str(); // const_cast<char*>( s.c_str() );
	  extractData(sdata, 'i',  binfile, fnumber, dur, stepsize );
	  sdata*=CurrentStimulus.vFactor; 
	}
	else if(CurrentStimulus.swVersion.compare("relacs")==0) {
	  s = DataBrowser::folder + filename + "/trace-1.raw"; //relacs
	  const char* binfile =  s.c_str(); //  const_cast<char*>( s.c_str() );
	  extractData(sdata, 'f',  binfile, fnumber, dur, stepsize);
	}
	else {
	  cerr << "Unknown software version!" << endl;
	}

	DataBrowser::currentTrace = sdata;
	
	
	DataBrowser::splot->clear();
	DataBrowser::splot->setXRange(0.0,sdata.size()*sdata.stepsize());
	
	DataBrowser::splot->plot(sdata, Plot::Yellow);
	
	
	// spike detection and polynomial fit
	
	ArrayD times;
	DataBrowser::detectSpikes(filename, fnumber, dur, stepsize, times, sdata);
	
	
	ArrayD correctedSpikes;
	DataBrowser::polyFit(times, sdata, stepsize, correctedSpikes, true);

	
	DataBrowser::splot->show();*/
      }
      else
	cerr << "Stimulus does not exist!" << endl;
    }
    else
      cerr << "Cell does not exist!" << endl;


  }
  else if(type==0) {

    //DataBrowser::splot->clear();

    map<string, map<int,DataBrowser::Rep>* >::iterator iterRepro;
    iterRepro=DataBrowser::Protocol->find(item->parent()->text(col).toStdString());

    if(iterRepro!=DataBrowser::Protocol->end()) {

      map<int,DataBrowser::Rep> * currentCell2 = iterRepro->second;

      map<int,DataBrowser::Rep>::iterator itr;
      
      QTreeWidgetItem *head = item->parent(); 
      itr=currentCell2->find(head->indexOfChild(item));
      //cout << "repro: " << head->indexOfChild(item) << endl;
      
      if(itr!=currentCell2->end()) {
	DataBrowser::Rep CurrentRepro = itr->second;

	Options Opt = CurrentRepro.Option;

	// Metadata options output
	//DataBrowser::metadata->assign(&Opt, 0, 1);
      }
      else
	cerr << "Repro does not exist!" << endl;
    }
    else
      cerr << "Cell does not exist!" << endl;
  }
  else if(type==-1) {

    //DataBrowser::splot->clear();

    map<string, DataBrowser::Cell* >::iterator iterExp;
    iterExp=DataBrowser::Header->find(item->text(col).toStdString());

    if(iterExp!=DataBrowser::Header->end()) {

      DataBrowser::Cell * CurrentExp = iterExp->second;

      Options Opt = CurrentExp->Head;

      // Metadata options output
      //DataBrowser::metadata->assign(&Opt, 0, 1);
    }
    else
      cerr << "Cell does not exist!" << endl;
  }
}



void DataBrowser::read(string cellname, QTreeWidgetItem *parent)
{

  int type = parent->type();

  if(type==-1) {

    map<long, DataBrowser::Stimulus> * NStimuli = new map<long, DataBrowser::Stimulus>;
    map<int, DataBrowser::Rep> * NRepro = new map<int, DataBrowser::Rep>;

    string file = folder + cellname + "/stimuli.dat";//relacs
    string version = "relacs";
    
    DataFile Sf;
    Sf.open( file );
    if ( !Sf.good() ) {
      file = folder + cellname + "/trigger.dat";//oel
      version="oel";
      Sf.open( file );
      if ( !Sf.good() ) {
	cerr << "can't open neither file stimuli.dat nor trigger.dat\n";
	return;
      }
    }
    cout << file << endl;
    
    Options Opt;
    Options Popt;
    
    double sampleInterval = 0.0;


    int count = 0;
    
    while ( Sf.read( 1 ) ) {
      
      //new experiment
      if ( Sf.newMetaData( 1 ) ) {

	Popt = Sf.metaDataOptions( 1 );
	sampleInterval = Popt.number("sample interval", 0); // oel
	if(sampleInterval<=0.0) {
	  sampleInterval = Popt.number("sample interval1", 0); // relacs;
	  version = "relacs";
	}
	
	Popt.setFlags(1);

	DataBrowser::Cell *Exp = new DataBrowser::Cell();
	Exp->Head=Popt;

	DataBrowser::Header->insert( pair<string, DataBrowser::Cell* >( cellname, Exp) );

      }

      // new repro:
      if ( Sf.newMetaData( 0 ) ) {
	Opt = Sf.metaDataOptions( 0 );
	string repro = Opt.text("repro", 0);
	//	cout << repro << endl;
	QTreeWidgetItem *child = new QTreeWidgetItem(QStringList(QObject::tr(repro.c_str())), 0);
	parent->addChild(child);
	
	Opt.setFlags(1);

	
	DataBrowser::Rep Re;
	Re.Option=Opt;

	NRepro->insert( pair<int,DataBrowser::Rep>( count, Re) );
	DataBrowser::Protocol->insert( pair<string, map< int, DataBrowser::Rep>* >( cellname, NRepro) );
	count++;

	TableKey Key = Sf.key();
	int index = Key.column("Left-Speaker>sound intensity");//relacs
	if(index < 0 || Sf.data(index,0)<0)
	  index = Key.column("Right-Speaker>sound intensity");//relacs
	if(index<0)
	  index = Key.column("stimulus-1>intensity");//oel
	
	TableKey Duration = Sf.key();
	int dur = Duration.column("Left-Speaker>duration");//relacs
	if(dur < 0 || Sf.data(dur,0)<0)
	  dur = Duration.column("Right-Speaker>duration");//relacs
	if(dur<0)
	  dur = Duration.column("duration");//oel;
	
	TableKey TraceIndex = Sf.key();
	//int traceI = Duration.column("index");//relacs & oel
	int time =  Duration.column("time");
	//double ind = time / (sampleInterval*0.001);
	//cout << ind << endl;
	//int traceI = (int) ind;//relacs & oel
	int factor = Duration.column("factor");//oel
	//cout << traceI << endl;
	
	
	ArrayD col = Sf.col(index);
	
	for (int i=0; i<col.size(); i++) {
	  //string s = "Sound intensity: " +  Str(Sf.data(index,i)-3.0,5,1,'f') + " dB SPL\tStimulus duration: " +  Str(Sf.data(dur,i),10,1,'f') + " ms";
	  string s = Str(Sf.data(index,i)-3.0,5,1,'f'); // dB SPL
	  s = s.append("  dB SPL");
	  long fnumber = Sf.data(time,i) / (sampleInterval*0.001);
	  //string s = Str(fnumber,10,0,'f');
	  
	  QTreeWidgetItem *cchild = new QTreeWidgetItem(QStringList(QObject::tr(s.c_str())), fnumber);
	  child->addChild(cchild);
	  //string unit = "dB SPL";
	  //cchild->setText(1, QString(QObject::tr(unit.c_str())));
	  
	  DataBrowser::Stimulus St;
	  St.name=cellname;
	  St.time=Sf.data(time,i);
	  St.index=fnumber;
	  St.duration=Sf.data(dur,i);
	  St.soundintensity=Sf.data(index,i);
	  St.stepsize=sampleInterval;
	  St.swVersion=version;
	  if(!(factor < 0 || Sf.data(factor,0)<0)) {
	    version = "oel";
	    St.vFactor=Sf.data(factor,i);
	  }
	  else {
	    St.vFactor=-1.0;
	    version = "relacs";
	  }
	  
	  
	  NStimuli->insert( pair<long,DataBrowser::Stimulus>( fnumber, St) );
	  
	  
	}
	
      }
      
    }
    
    DataBrowser::Cells->insert( pair<string, map< long, DataBrowser::Stimulus>* >( cellname, NStimuli) );
    
    //treeWidget->collapseItem(parent);
    
  }
}



}; /* namespace relacs */


#include "moc_databrowser.cc"
