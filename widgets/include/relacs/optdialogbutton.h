/*
  optdialogbutton.h
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_OPTDIALOGBUTTON_H_
#define _RELACS_OPTDIALOGBUTTON_H_ 1

#include <vector>
#include <QPushButton>
#include "optdialog.h"
using namespace std;

namespace relacs {


class OptDialogButton : public QObject
{
  Q_OBJECT

public:

  OptDialogButton( void );
  OptDialogButton( const OptDialogButton &odb );
  OptDialogButton( const string &title, OptDialog::Action action,
		   int rcode, bool close, bool dflt );
  ~OptDialogButton( void );

  void create( vector<OptWidget*> ows, int rejectc,
	       QHBoxLayout *layout );

  bool defaultButton( void ) const;
  void setDefaultButton( bool d=true );
  bool accept( void ) const;

public slots:

  void clicked( void );

signals:

  void accepted( void );
  void clicked( int rcode );
  void done( int rcode );


private:
  
  string Title;
  OptDialog::Action Action;
  int ReturnCode;
  bool Close;
  bool Default;
  vector < OptWidget* > OWs;
  QPushButton *PB;
 
};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTDIAlOGBUTTON_H_ */

