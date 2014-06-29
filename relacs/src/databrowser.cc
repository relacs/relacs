/*
  databrowser.cc
  Interface for browsing previously recorded data.

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

#include <relacs/repro.h>
#include <relacs/dataindex.h>
#include <relacs/databrowser.h>


namespace relacs {


DataBrowser::DataBrowser( DataIndex *data, QWidget *parent )
  : QSplitter( parent )
{
  OverviewWidget = new QTreeView;
  addWidget( OverviewWidget );
  OverviewWidget->setModel( data->overviewModel() );
  data->setOverviewView( OverviewWidget, this );

  DescriptionWidget = new QTreeView;
  addWidget( DescriptionWidget );
  DescriptionWidget->setModel( data->descriptionModel() );
  data->setDescriptionView( DescriptionWidget );
}


DataBrowser::~DataBrowser( void )
{
}


void DataBrowser::keyPressEvent( QKeyEvent *qke )
{
  // this eats up all left over key presses from the views.
}


  /*
void DataBrowser::list( QTreeWidgetItem * item, int col )
{
  int type = item->type();
  //cout << "Typ: " << type << endl;

  map< string, map< long, Stimulus >* >::iterator iterCell;
  iterCell = Cells->find( item->text( col ).toStdString() );


  if( type<0 && iterCell == Cells->end() ) {
    read( item->text(0).toStdString(), item );
    item->setExpanded( true );
  }


  //bin2dat
  if(type>0) {

  QTreeWidgetItem *parent = item->parent();
    cout << parent->indexOfChild(item) << ": plot!\n";

    long fnumber = type; //item->text(col).toLong();

    map<string, map<long,Stimulus>* >::iterator iterCell;
    iterCell=Cells->find(parent->parent()->text(col).toStdString());

    map<string, map<int,Rep>* >::iterator iterRepro;
    iterRepro=Protocol->find(parent->parent()->text(col).toStdString());

    if(iterCell!=Cells->end() && iterRepro!=Protocol->end()) {
      map<long,Stimulus> * currentCell = iterCell->second;

      map<long,Stimulus>::iterator it;
      it=currentCell->find(fnumber);

      map<int,Rep> * currentCell2 = iterRepro->second;

      map<int,Rep>::iterator itr;
      
      QTreeWidgetItem *head = item->parent();
      itr=currentCell2->find(head->parent()->indexOfChild(head));
      // cout << "repro: " << head->parent()->indexOfChild(head) << endl;
      
      if(it!=currentCell->end() && itr!=currentCell2->end()) {
	Stimulus CurrentStimulus = it->second;
	Rep CurrentRepro = itr->second;
	
	string filename = CurrentStimulus.name;
	// double dur = CurrentStimulus.duration;
	// double stepsize = CurrentStimulus.stepsize;

	Options Opt = CurrentRepro.Option;

	// Metadata options output
	//metadata->assign(&opt, 0, 1);
		
	
	/ * SampleDataD sdata(0.0, dur, stepsize);

	string s;
	if(CurrentStimulus.swVersion.compare("oel") == 0) {
	  s = Folder + filename + "/traces.sw1"; //oel
	  const char* binfile =  s.c_str(); // const_cast<char*>( s.c_str() );
	  extractData(sdata, 'i',  binfile, fnumber, dur, stepsize );
	  sdata*=CurrentStimulus.vFactor;
	}
	else if(CurrentStimulus.swVersion.compare("relacs") == 0) {
	  s = Folder + filename + "/trace-1.raw"; //relacs
	  const char* binfile =  s.c_str(); //  const_cast<char*>( s.c_str() );
	  extractData(sdata, 'f',  binfile, fnumber, dur, stepsize);
	}
	else {
	  cerr << "Unknown software version!" << endl;
	}

	currentTrace = sdata;
	
	
	splot->clear();
	splot->setXRange(0.0,sdata.size()*sdata.stepsize());
	
	splot->plot(sdata, Plot::Yellow);
	
	
	// spike detection and polynomial fit
	
	ArrayD times;
	detectSpikes(filename, fnumber, dur, stepsize, times, sdata);
	
	
	ArrayD correctedSpikes;
	polyFit(times, sdata, stepsize, correctedSpikes, true);

	
	splot->show(); * /
      }
      else
	cerr << "Stimulus does not exist!" << endl;
    }
    else
      cerr << "Cell does not exist!" << endl;


  }
  else if( type == 0 ) {

    //splot->clear();

    map<string, map<int,Rep>* >::iterator iterRepro;
    iterRepro=Protocol->find(item->parent()->text(col).toStdString());

    if(iterRepro!=Protocol->end()) {

      map<int,Rep> * currentCell2 = iterRepro->second;

      map<int,Rep>::iterator itr;
      
      QTreeWidgetItem *head = item->parent();
      itr=currentCell2->find(head->indexOfChild(item));
      //cout << "repro: " << head->indexOfChild(item) << endl;
      
      if(itr!=currentCell2->end()) {
	Rep CurrentRepro = itr->second;

	Options Opt = CurrentRepro.Option;

	// Metadata options output
	//metadata->assign(&Opt, 0, 1);
      }
      else
	cerr << "Repro does not exist!" << endl;
    }
    else
      cerr << "Cell does not exist!" << endl;
  }
  else if( type == -1 ) {

    //splot->clear();

    map<string, Cell* >::iterator iterExp;
    iterExp=Header->find(item->text(col).toStdString());

    if(iterExp!=Header->end()) {

      Cell * CurrentExp = iterExp->second;

      Options Opt = CurrentExp->Head;

      // Metadata options output
      //metadata->assign(&Opt, 0, 1);
    }
    else
      cerr << "Cell does not exist!" << endl;
  }
}



void DataBrowser::read( string cellname, QTreeWidgetItem *parent )
{

  int type = parent->type();

  if( type == -1 ) {

    map<long, Stimulus> * NStimuli = new map<long, Stimulus>;
    map<int, Rep> * NRepro = new map<int, Rep>;

    string file = Folder + "/" + cellname + "/stimuli.dat";//relacs
    string version = "relacs";
    
    DataFile sf;
    sf.open( file );
    if ( !sf.good() ) {
      file = Folder + "/" + cellname + "/trigger.dat";//oel
      version="oel";
      sf.open( file );
      if ( !sf.good() ) {
	cerr << "can't open file stimuli.dat or trigger.dat\n";
	return;
      }
    }
    cout << "read " << file << endl;
    
    Options Opt;
    Options Popt;
    
    double sampleInterval = 0.0;


    int count = 0;
    
    while ( sf.read( 1 ) ) {
      
      //new experiment
      if ( sf.newMetaData( 1 ) ) {

	Popt = sf.metaDataOptions( 1 );
	sampleInterval = Popt.number("sample interval", 0); // oel
	if(sampleInterval<=0.0) {
	  sampleInterval = Popt.number("sample interval1", 0); // relacs;
	  version = "relacs";
	}
	
	Popt.setFlags(1);

	Cell *Exp = new Cell();
	Exp->Head=Popt;

	Header->insert( pair<string, Cell* >( cellname, Exp) );

      }

      // new repro:
      if ( sf.newMetaData( 0 ) ) {
	Opt = sf.metaDataOptions( 0 );
	string repro = Opt.text("repro", 0);
	//	cout << repro << endl;
	QTreeWidgetItem *child = new QTreeWidgetItem(QStringList(QObject::tr(repro.c_str())), 0);
	parent->addChild(child);
	
	Opt.setFlags(1);

	
	Rep Re;
	Re.Option=Opt;

	NRepro->insert( pair<int,Rep>( count, Re) );
	Protocol->insert( pair<string, map< int, Rep>* >( cellname, NRepro) );
	count++;

	TableKey Key = sf.key();
	int index = Key.column("Left-Speaker>sound intensity");//relacs
	if(index < 0 || sf.data(index,0)<0)
	  index = Key.column("Right-Speaker>sound intensity");//relacs
	if(index<0)
	  index = Key.column("stimulus-1>intensity");//oel
	
	TableKey Duration = sf.key();
	int dur = Duration.column("Left-Speaker>duration");//relacs
	if(dur < 0 || sf.data(dur,0)<0)
	  dur = Duration.column("Right-Speaker>duration");//relacs
	if(dur<0)
	  dur = Duration.column("duration");//oel;
	
	TableKey TraceIndex = sf.key();
	//int traceI = Duration.column("index");//relacs & oel
	int time =  Duration.column("time");
	//double ind = time / (sampleInterval*0.001);
	//cout << ind << endl;
	//int traceI = (int) ind;//relacs & oel
	int factor = Duration.column("factor");//oel
	//cout << traceI << endl;
	
	
	ArrayD col = sf.col(index);
	
	for (int i=0; i<col.size(); i++) {
	  //string s = "Sound intensity: " +  Str(sf.data(index,i)-3.0,5,1,'f') + " dB SPL\tStimulus duration: " +  Str(sf.data(dur,i),10,1,'f') + " ms";
	  string s = Str(sf.data(index,i)-3.0,5,1,'f'); // dB SPL
	  s = s.append("  dB SPL");
	  long fnumber = sf.data(time,i) / (sampleInterval*0.001);
	  //string s = Str(fnumber,10,0,'f');
	  
	  QTreeWidgetItem *cchild = new QTreeWidgetItem(QStringList(QObject::tr(s.c_str())), fnumber);
	  child->addChild(cchild);
	  //string unit = "dB SPL";
	  //cchild->setText(1, QString(QObject::tr(unit.c_str())));
	  
	  Stimulus St;
	  St.name=cellname;
	  St.time=sf.data(time,i);
	  St.index=fnumber;
	  St.duration=sf.data(dur,i);
	  St.soundintensity=sf.data(index,i);
	  St.stepsize=sampleInterval;
	  St.swVersion=version;
	  if(!(factor < 0 || sf.data(factor,0)<0)) {
	    version = "oel";
	    St.vFactor=sf.data(factor,i);
	  }
	  else {
	    St.vFactor=-1.0;
	    version = "relacs";
	  }
	  
	  
	  NStimuli->insert( pair<long,Stimulus>( fnumber, St ) );
	  
	  
	}
	
      }
      
    }
    
    Cells->insert( pair<string, map< long, Stimulus>* >( cellname, NStimuli) );
    
    //treeWidget->collapseItem(parent);
    
  }
}

*/


void DataBrowser::display( const string &path, const deque<int> &traceindex,
			   const deque<int> &eventsindex, double time )
{
  emit displayIndex( path, traceindex, eventsindex, time );
}


DataOverviewModel::DataOverviewModel( QObject *parent )
  : QAbstractItemModel( parent ),
    Data( 0 ),
    View( 0 ),
    Browser( 0 ),
    AutoActivate( false )
{
}


void DataOverviewModel::setDataIndex( DataIndex *data )
{
  Data = data;
}


void DataOverviewModel::setTreeView( QTreeView *view )
{
  View = view;
  if ( View != 0 ) {
    connect( view->selectionModel(),
	     SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
	     this, SLOT( setDescription( const QModelIndex&, const QModelIndex& ) ) );
  }
}


void DataOverviewModel::setBrowser( DataBrowser *browser )
{
  Browser = browser;
}


QVariant DataOverviewModel::data( const QModelIndex &index, int role ) const
{
  if ( ! index.isValid() )
    return QVariant();

  if ( role != Qt::DisplayRole )
    return QVariant();

  if ( index.column() > 0 )
    return QVariant();

  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );

  if ( item->level() == 1 ) {
    Str file = item->name();
    return QVariant( QString( file.dir().preventedSlash().name().c_str() ) );
  }
  else if ( item->level() == 3 ) {
    Str stimulus = item->name();
    stimulus.eraseFirst( "stimulus/" );
    return QVariant( QString( stimulus.c_str() ) );
  }
  return QVariant( QString( item->name().c_str() ) );
}


Qt::ItemFlags DataOverviewModel::flags( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DataOverviewModel::headerData( int section, Qt::Orientation orientation,
					int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return QVariant( QString( "Name" ) );

  return QVariant();
}


QModelIndex DataOverviewModel::index( int row, int column,
				      const QModelIndex &parent ) const
{
  //  if ( !hasIndex( row, column, parent ) )
  //    return QModelIndex();

  DataIndex::DataItem *parentitem = 0;
  if ( ! parent.isValid() )
    parentitem = Data->cells();
  else
    parentitem = static_cast<DataIndex::DataItem*>( parent.internalPointer() );

  DataIndex::DataItem *childitem = parentitem->child( row );
  if ( childitem )
    return createIndex( row, column, childitem );

  return QModelIndex();
}


QModelIndex DataOverviewModel::parent( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return QModelIndex();

  DataIndex::DataItem* childitem =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );
  DataIndex::DataItem* parentitem = childitem->parent();
  if ( parentitem == Data->cells() )
    return QModelIndex();
  return createIndex( parentitem->parent()->index( parentitem ), 0, parentitem );
}


bool DataOverviewModel::hasChildren( const QModelIndex &parent ) const
{
  if ( ! parent.isValid() )
    return ! Data->cells()->empty();

  DataIndex::DataItem* parentitem =
    static_cast<DataIndex::DataItem*>( parent.internalPointer() );
  if ( parentitem == 0 )
    return false;

  if ( parentitem->level() == 1 )
    return true;
  else
    return ! parentitem->empty();
}


int DataOverviewModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return 0;

  if ( parent.isValid() ) {
    DataIndex::DataItem* parentitem =
      static_cast<DataIndex::DataItem*>( parent.internalPointer() );
    if ( parentitem->level() == 1 ) {
      //      cerr << "we need to load cell " << parentitem->name() << '\n';
    }
    return parentitem->size();
  }
  else
    return Data->size();

  return 0;
}


int DataOverviewModel::columnCount( const QModelIndex &parent ) const
{
  return 1;
}


bool DataOverviewModel::canFetchMore( const QModelIndex &parent ) const
{
  if ( ! parent.isValid() )
    return false;

  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( parent.internalPointer() );

  if ( item->level() == 1 && item->size() == 0 )
    return true;

  return false;
}


void DataOverviewModel::fetchMore( const QModelIndex &parent )
{
  if ( ! parent.isValid() )
    return;

  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( parent.internalPointer() );

  item->loadCell();
}


void DataOverviewModel::beginAddChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginInsertRows( QModelIndex(), parent->size(), parent->size() );
  else
    beginInsertRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size(), parent->size() );
}


  void DataOverviewModel::endAddChild( DataIndex::DataItem *parent, bool showlast )
{
  endInsertRows();
  if ( View != 0 ) {
    if ( parent->size() > 1 ) {
      DataIndex::DataItem *child = parent->child( parent->size()-2 );
      View->collapse( createIndex( parent->size()-2, 0, child ) );
      while ( child->level() < 3 && ! child->empty() ) {
	DataIndex::DataItem *item = child;
	child = item->child( item->size()-1 );
	View->collapse( createIndex( item->size()-1, 0, child ) );
      }
    }
    QModelIndex item = createIndex( 0, 0, parent->child( 0 ) );
    if ( showlast )
      item = createIndex( parent->size()-1, 0,
			  parent->child( parent->size()-1 ) );
    AutoActivate = true;
    View->expand( item );
    View->scrollTo( item );
    View->setCurrentIndex( item );
    AutoActivate = false;
  }
}


void DataOverviewModel::beginPopChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginRemoveRows( QModelIndex(), parent->size()-1, parent->size()-1 );
  else
    beginRemoveRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size()-1, parent->size()-1 );
}


void DataOverviewModel::endPopChild( DataIndex::DataItem *parent )
{
  endRemoveRows();
  if ( View != 0 ) {
    QModelIndex item = createIndex( parent->size()-1, 0,
				    parent->child( parent->size()-1 ) );
    AutoActivate = true;
    View->scrollTo( item );
    View->setCurrentIndex( item );
    AutoActivate = false;
  }
}


void DataOverviewModel::setDescription( const QModelIndex &index )
{
  if ( ! index.isValid() )
    return;

  if ( index.column() > 0 )
    return;

  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );

  if ( item == 0 )
    return;

  // inform descriptionModel about new selected data:
  Data->descriptionModel()->setOptions( &item->data() );
}


void DataOverviewModel::setDescription( const QModelIndex &current,
					const QModelIndex &previous )
{
  setDescription( current );

  // plot selected data:
  displayIndex( current );
}


void DataOverviewModel::displayIndex( const QModelIndex &index )
{
  // We only display the selection if the user activated it:
  if ( AutoActivate )
    return;

  if ( ! index.isValid() )
    return;

  if ( index.column() > 0 )
    return;

  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );

  if ( item == 0 )
    return;

  // no stimulus:
  if ( item->level() < 3 )
    return;

  // display stimulus:
  if ( Browser != 0 )
    Browser->display( item->fileName(), item->traceIndex(), item->eventsIndex(), item->time() );
}


DataDescriptionModel::DataDescriptionModel( QObject *parent )
  : QAbstractItemModel( parent ),
    Data( 0 ),
    View( 0 )
{
}


void DataDescriptionModel::setOptions( Options *data )
{
  beginResetModel();
  Data = data;
  endResetModel();
  if ( Data != 0 )
    View->expandToDepth( 1 );
}


void DataDescriptionModel::setTreeView( QTreeView *view )
{
  View = view;
}


QVariant DataDescriptionModel::data( const QModelIndex &index, int role ) const
{
  if ( ! index.isValid() )
    return QVariant();

  if ( role != Qt::DisplayRole )
    return QVariant();

  if ( index.column() > 1 )
    return QVariant();

  Options *item = static_cast<Options*>( index.internalPointer() );

  string s = "";
  int i = index.row();
  if ( i < item->parameterSize() )
    s = index.column() > 0 ? (*item)[i].text( "%s" ) : (*item)[i].name();
  else {
    i -= item->parameterSize();
    if ( i < item->sectionsSize() )
      s = index.column() > 0 ? item->section( i ).type() : item->section( i ).name();
  }

  return QVariant( QString( s.c_str() ) );
}


Qt::ItemFlags DataDescriptionModel::flags( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DataDescriptionModel::headerData( int section, Qt::Orientation orientation,
					   int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return section > 0 ? QVariant( QString( "Value" ) ) : QVariant( QString( "Name" ) );

  return QVariant();
}


QModelIndex DataDescriptionModel::index( int row, int column,
					 const QModelIndex &parent ) const
{
  //  if ( !hasIndex( row, column, parent ) )
  //    return QModelIndex();

  Options *parentitem = 0;
  if ( ! parent.isValid() ) {
    if ( Data != 0 )
      return createIndex( row, column, Data );
    else
      return QModelIndex();
  }

  parentitem = static_cast<Options*>( parent.internalPointer() );
  if ( parent.row() >= parentitem->parameterSize() ) {
    int r = parent.row() - parentitem->parameterSize();
    if ( r < parentitem->sectionsSize() ) {
      Options *childitem = &parentitem->section( r );
      return createIndex( row, column, childitem );
    }
  }

  return QModelIndex();
}


QModelIndex DataDescriptionModel::parent( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return QModelIndex();

  Options* childitem = static_cast<Options*>( index.internalPointer() );
  Options* parentitem = childitem->parentSection();
  if ( parentitem == 0 )
    return QModelIndex();

  for ( int r=0; r<parentitem->sectionsSize(); r++ ) {
    if ( &parentitem->section( r ) == childitem ) {
      r += parentitem->parameterSize();
      return createIndex( r, 0, parentitem );
    }
  }
  return QModelIndex();
}


bool DataDescriptionModel::hasChildren( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return false;

  if ( parent.isValid() ) {
    Options* parentitem = static_cast<Options*>( parent.internalPointer() );
    int r = parent.row() -  parentitem->parameterSize();
    if ( r >= 0 && r < parentitem->sectionsSize() ) {
      Options* childitem = &parentitem->section( r );
      return ( childitem->parameterSize() + childitem->sectionsSize() ) > 0;
    }
  }
  else if ( Data != 0 )
    return ( Data->parameterSize() + Data->sectionsSize() ) > 0;

  return false;
}


int DataDescriptionModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return 0;

  if ( parent.isValid() ) {
    Options* parentitem = static_cast<Options*>( parent.internalPointer() );
    int r = parent.row() -  parentitem->parameterSize();
    if ( r >= 0 && r < parentitem->sectionsSize() ) {
      Options* childitem = &parentitem->section( r );
      return childitem->parameterSize() + childitem->sectionsSize();
    }
  }
  else if ( Data != 0 )
    return Data->parameterSize() + Data->sectionsSize();

  return 0;
}


int DataDescriptionModel::columnCount( const QModelIndex &parent ) const
{
  return 2;
}


}; /* namespace relacs */


#include "moc_databrowser.cc"
