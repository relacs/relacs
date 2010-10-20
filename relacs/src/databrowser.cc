/*
  databrowser.cc
  Interface for browsing previously recorded data.

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

#include <QHBoxLayout>
#include <relacs/repro.h>
#include <relacs/databrowser.h>

namespace relacs {


DataBrowser::DataBrowser( QWidget *parent )
  : QWidget( parent )
{
  TreeWidget = new QTreeWidget;
  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget( TreeWidget );
  setLayout( layout );
}


DataBrowser::~DataBrowser( void )
{
}


void DataBrowser::addSession( const string &path )
{
  TreeWidget->addTopLevelItem( new QTreeWidgetItem( (QTreeWidget*)0,
						    QStringList( path.c_str() ) ) );
}


void DataBrowser::addRepro( const RePro *repro )
{
  if ( TreeWidget->topLevelItemCount() > 0 )
    TreeWidget->topLevelItem( TreeWidget->topLevelItemCount()-1 )->addChild( new QTreeWidgetItem( (QTreeWidget*)0, QStringList( repro->name().c_str() ) ) );
}


}; /* namespace relacs */


#include "moc_databrowser.cc"
