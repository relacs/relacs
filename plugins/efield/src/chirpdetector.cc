/*
  efield/chirpdetector.cc
  Detects chirps of wave-type weakly electric fish

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

#include <relacs/efield/chirpdetector.h>
using namespace relacs;

namespace efield {


ChirpDetector::ChirpDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleEventDetector, 1,
	    "ChirpDetector", "Chirp Detector", "EField", 
	    "Jan Benda", "1.2", "Jun 17, 2009" ),
    CDW( (QWidget*)this )
{
  // parameter:
  Threshold = 8.0;
  MinThresh = 0.0;
  ChirpMinWidth = 0.003;
  ChirpMaxWidth = 0.05;
  ChirpCycles = 20;
  AverageCycles = 40;
  Other = 0;

  // options:
  addNumber( "threshold", "Threshold", Threshold, 0.0, 2000.0, 2.0, "Hz", "Hz", "%.0f", 2+32 );
  addNumber( "minthresh", "Minimum threshold", MinThresh, 0.0, 1000.0, 2.0, "Hz", "Hz", "%.0f", 8+32 );
  addNumber( "minwidth", "Minimum width", ChirpMinWidth, 0.0, 0.5, 0.002, "s", "ms", "%.0f", 2+32 );
  addNumber( "maxwidth", "Maximum width", ChirpMaxWidth, 0.002, 0.5, 0.002, "s", "ms", "%.0f", 2+32 );
  addNumber( "rate", "Rate", 0.0, 0.0, 2000.0, 1.0, "Hz", "Hz", "%.0f", 2+4 );
  addNumber( "size", "Size", 0.0, 0.0, 2000.0, 1.0, "Hz", "Hz", "%.0f", 2+4 );
  addNumber( "width", "Width", 0.0, 0.0, 1000.0, 0.1, "ms", "ms", "%.0f", 2+4 );
  addStyle( OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, 4 );

  CDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  CDW.setSpacing( 4 );
  CDW.setMargin( 4 );
  connect( this, SIGNAL( dialogAccepted( void ) ),
	   &CDW, SLOT( updateValues( void ) ) );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );
}


ChirpDetector::~ChirpDetector( void )
{
}


int ChirpDetector::init( const EventData &inevents, EventData &outevents,
			 const EventList &other, const EventData &stimuli )
{
  D.init( EventFrequencyIterator( inevents.begin() + 1 ),
	  EventFrequencyIterator( inevents.end() ),
	  EventIterator( inevents.begin() + 1 ) );

  Other = &other;

  return 0;
}


void ChirpDetector::notify( void )
{
  Threshold = number( "threshold" );
  MinThresh = number( "minthresh" );
  ChirpMinWidth = number( "minwidth" );
  ChirpMaxWidth = number( "maxwidth" );

  if ( Threshold < MinThresh ) {
    Threshold = MinThresh;
    setNumber( "threshold", Threshold );
  }
  CDW.updateValues( OptWidget::changedFlag() );
}


int ChirpDetector::detect( const EventData &inevents, EventData &outevents,
			    const EventList &other, const EventData &stimuli )
{
  long lastsize = outevents.size();

  D.peak( EventFrequencyIterator( inevents.begin() + 1 ), 
	  EventFrequencyIterator( inevents.end() ),
	  outevents, Threshold, MinThresh, 2.0*Threshold, *this );

  if ( outevents.size() - lastsize <= 0 )
    outevents.updateMean();

  unsetNotify();
  setNumber( "rate", outevents.meanRate() );
  setNumber( "size", outevents.meanSize() );
  setNumber( "width", outevents.meanWidth() );
  setNotify();
  CDW.updateValues( OptWidget::changedFlag() );
  return 0;
}


int ChirpDetector::checkEvent( const EventFrequencyIterator &first, 
			       const EventFrequencyIterator &last,
			       EventFrequencyIterator &event, 
			       EventIterator &eventtime, 
			       EventFrequencyIterator &index,
			       EventIterator &indextime,
			       EventFrequencyIterator &prevevent, 
			       EventIterator &prevtime, 
			       EventData &outevents, 
			       double &threshold,
			       double &minthresh, double &maxthresh,
			       double &time, double &size, double &width )
{
  // chirp too long:
  if ( index.time() - event.time() > ChirpMaxWidth )
    return 0;

  // store event:
  EventFrequencyIterator orgevent = event;

  // meanrate before chirp:
  double meanrate = 0.0;
  EventFrequencyIterator cindex = event - ChirpCycles;
  for ( int j=0; j<AverageCycles && !cindex; j++ ) {
    meanrate += ( *cindex - meanrate )/(j+1);
    --cindex;
  }

  // find end of chirp (ChirpCycles data points within +- 1/2 threshold in a row):
  EventFrequencyIterator findex;
  int n = 0;
  double rate = *index;
  for ( findex = index+1; !findex; ++findex ) {
    if ( *event < *findex )
      event = findex;
    if ( fabs( *findex - rate ) < 0.5*threshold ) 
      n++;
    else {
      n = 0;
      rate = *findex;
    }
    if ( n > ChirpCycles )
      break;
  }

  // end of chirp not contained in data:
  if ( !(!findex) || *findex > rate + 0.5*threshold )
    return -1;

  // size of chirp:
  size = *event - meanrate;
  if ( size < MinThresh )
    return 0;

  // chirp:
  time = event.time();
  double minrate = meanrate + 0.1 * size;
  index = findex;

  // find begin of chirp:
  EventFrequencyIterator lindex;
  if ( *orgevent > minrate ) {
    for ( lindex = orgevent-1; !lindex; --lindex )
      if ( *lindex <= minrate )
	break;
  }
  else {
    for ( lindex = orgevent+1; !lindex; ++lindex )
      if ( *lindex >= minrate )
	break;
  }

  // lindex not valid:
  if ( !(!lindex) )
    return -1;

  // find end of chirp:
  for ( --findex; !findex && findex > event; --findex )
    if ( *findex >= minrate )
      break;

  width = findex.time() - lindex.time();

  // chirp too short:
  if ( width < ChirpMinWidth )
    return 0;

  // chirp too long:
  if ( width > ChirpMaxWidth )
    return 0;

  // check for chirps in the other event traces:
  for ( int k=0; k<Other->size(); k++ )
    if ( (*Other)[k].within( time, width ) )
      return 0;

  // this is a chirp which occurs only in this trace:
  return 1;
}


addDetector( ChirpDetector );

}; /* namespace efield */

#include "moc_chirpdetector.cc"
