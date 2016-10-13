/*
  dynclampmodelsim.cc
  Interface to the dynamic clamp model.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#include <math.h>
#include <relacs/parameter.h>
#include <relacs/rtaicomedi/moduledef.h>
#include <relacs/rtaicomedi/dynclampmodelsim.h>


namespace relacs {


namespace dynclampmodelsim {

vector<int> modelaitraces;
vector<int> modelaotraces;

#ifdef ENABLE_COMPUTATION

#ifdef ENABLE_LOOKUPTABLES
int lookupinx = 0;
int lookupn[MAXLOOKUPTABLES];
float* lookupx[MAXLOOKUPTABLES];
float* lookupy[MAXLOOKUPTABLES];
#endif

#define DYNCLAMPMODEL
#include "../module/model.c"
#undef DYNCLAMPMODEL

#endif

void generateLookupTables( void )
{
#ifdef ENABLE_COMPUTATION
#ifdef ENABLE_LOOKUPTABLES
  for ( int k=0; ; k++ ) {
    lookupx[k] = 0;
    lookupy[k] = 0;
    lookupn[k] = 0;
    if ( generateLookupTable( k, &lookupx[k], &lookupy[k], &lookupn[k] ) < 0 ) 
      break;
  }
#endif
#endif
}


string initStatus( vector<float> &statusInput,
		   vector<string> &statusInputNames, vector<string> &statusInputUnits )
{
  // parameter channel offset:
  if ( PARAM_CHAN_OFFSET != InData::ParamChannel )
    return "PARAM_CHAN_OFFSET=" + Str( PARAM_CHAN_OFFSET ) +
           " from moduledef.h does not equal ParamChannel=" + Str( InData::ParamChannel ) +
	   " from analoginput.h";

#ifdef ENABLE_COMPUTATION
  for ( int k=0; k<OUTPUT_N; k++ ) {
    statusInputNames.push_back( string( "Stimulus-" ) + outputNames[k] );
    statusInputUnits.push_back( outputUnits[k] );
    statusInput.push_back( 0.0 );
    statusInputNames.push_back( string( "Model-" ) + outputNames[k] );
    statusInputUnits.push_back( outputUnits[k] );
    statusInput.push_back( 0.0 );
    statusInputNames.push_back( string( "Total-" ) + outputNames[k] );
    statusInputUnits.push_back( outputUnits[k] );
    statusInput.push_back( 0.0 );
#ifdef ENABLE_SYNCSEC
    statusInputNames.push_back( string( "Injected-" ) + outputNames[k] );
    statusInputUnits.push_back( outputUnits[k] );
    statusInput.push_back( 0.0 );
#endif
  }
#endif
  return "";
}


void initModel( float stepsize )
{
#ifdef ENABLE_COMPUTATION
  loopInterval = stepsize;
  loopRate = 1.0/loopRate;

  initModel();
#endif
}


void computeModel( InList &data,
		   const vector< int > &aochannels, vector< float > &aovalues,
		   int outputstatusinx, vector<float> &statusInput )
{
#ifdef ENABLE_COMPUTATION
  for ( int k=0; k<data.size(); k++ ) {
    int inx = modelaitraces[data[k].trace()];
    if ( inx >= 0  )
      input[ inx ] = data[k].back();
  }
  for ( unsigned int k=0; k<aochannels.size(); k++ ) {
    if ( aochannels[k] >= PARAM_CHAN_OFFSET )
      paramOutput[ aochannels[k]-PARAM_CHAN_OFFSET ] = aovalues[k];      
  }

  computeModel();
  
#ifdef ENABLE_SYNCSEC
    #define NSTATUS 4
#else
    #define NSTATUS 3
#endif
  for ( int k=0; k<OUTPUT_N; k++ ) {
    statusInput[outputstatusinx+NSTATUS*k+1] = output[k];
    statusInput[outputstatusinx+NSTATUS*k+2] = output[k];
    for ( unsigned int j=0; j<aochannels.size(); j++ ) {
      if ( outputChannels[k] == aochannels[j] ) {
	statusInput[outputstatusinx+NSTATUS*k] = aovalues[j];
	statusInput[outputstatusinx+NSTATUS*k+2] += aovalues[j];
	break;
      }
    }
#ifdef ENABLE_SYNCSEC
    statusInput[outputstatusinx+NSTATUS*k+3] = statusInput[outputstatusinx+NSTATUS*k+2];
#endif
  }
  for ( int k=0; k<data.size(); k++ ) {
    if ( data[k].channel() >= PARAM_CHAN_OFFSET &&
	 data[k].channel() < 2*PARAM_CHAN_OFFSET )
      data[k].push( paramInput[ data[k].channel()-PARAM_CHAN_OFFSET ] );
  }
  for ( unsigned int k=0; k<aochannels.size(); k++ ) {
    if ( aochannels[k] < PARAM_CHAN_OFFSET )
      aovalues[k] = output[ modelaotraces[ k ] ];
    else
      aovalues[k] = 0.0;
  }
#else
  for ( unsigned int k=0; k<aochannels.size(); k++ )
    aovalues[k] = 0.0;
#endif
}


void addAITraces( vector< TraceSpec > &traces, int deviceid )
{
#ifdef ENABLE_COMPUTATION
  int channel = PARAM_CHAN_OFFSET;
  for ( int k=0; k<PARAMINPUT_N; k++ ) {
    traces.push_back( TraceSpec( traces.size(), paramInputNames[k],
				 deviceid, channel++, 1.0, paramInputUnits[k] ) );
  }
#endif
}


void addAOTraces( vector< TraceSpec > &traces, int deviceid )
{
#ifdef ENABLE_COMPUTATION
  int channel = PARAM_CHAN_OFFSET;
  for ( int k=0; k<PARAMOUTPUT_N; k++ ) {
    traces.push_back( TraceSpec( traces.size(), paramOutputNames[k],
				 deviceid, channel++, 1.0, paramOutputUnits[k],
				 paramOutput[k] ) );
  }
#endif
}


int matchAITraces( InList &traces, const vector<string> &statusInputNames,
		   const vector<string> &statusInputUnits )
{
  int foundtraces = 0;
  bool tracefound[ traces.size() ];
  for ( int k=0; k<traces.size(); k++ )
    tracefound[k] = false;

  modelaitraces.resize( traces.size() );
  for ( unsigned int k=0; k<modelaitraces.size(); k++ )
    modelaitraces[k] = -1;

#ifdef ENABLE_COMPUTATION
  string unknowntraces = "";
  for ( int j=0; j<INPUT_N; j++ ) {
    bool notfound = true;
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() < PARAM_CHAN_OFFSET && traces[k].ident() == inputNames[j] ) {
	tracefound[k] = true;
	modelaitraces[traces[k].trace()] = j;
	if ( traces[k].unit() != inputUnits[j] )
	  traces[k].addErrorStr( "model input trace " + traces[k].ident() + " requires as unit '" + inputUnits[j] + "', not '" + traces[k].unit() + "'" );
	notfound = false;
	foundtraces++;
	break;
      }
    }
    if ( notfound ) {
      unknowntraces += " ";
      unknowntraces += inputNames[j];
    }
    
  }
  if ( ! unknowntraces.empty() )
    traces.addErrorStr( "unable to match model input traces" + unknowntraces );

  // parameter traces:
  for ( int j=0; j<PARAMINPUT_N; j++ ) {
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() >= PARAM_CHAN_OFFSET && traces[k].ident() == paramInputNames[j] ) {
	tracefound[k] = true;
	if ( traces[k].unit() != paramInputUnits[j] )
	  traces[k].addErrorStr( "model input parameter trace " + traces[k].ident() + " requires as unit '" + paramInputUnits[j] + "', not '" + traces[k].unit() + "'" );
	traces[k].setChannel( PARAM_CHAN_OFFSET + j );
	foundtraces++;
	break;
      }
    }
  }
#endif

  // status traces:
  for ( unsigned int j=0; j<statusInputNames.size(); j++ ) {
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() >= PARAM_CHAN_OFFSET && traces[k].ident() == statusInputNames[j] ) {
	tracefound[k] = true;
	double scaleval = Parameter::changeUnit( 1.0, statusInputUnits[j], traces[k].unit() );
	if ( traces[k].unit() != statusInputUnits[j] &&
	     ::fabs( traces[k].scale() - scaleval ) > 1e-8 )
	  traces[k].addErrorStr( "model input parameter trace " + traces[k].ident() +
				 " requires as unit '" + statusInputUnits[j] +
				 "', not '" + traces[k].unit() + "'" );
	traces[k].setChannel( 2*PARAM_CHAN_OFFSET + j );
	foundtraces++;
	break;
      }
    }
  }

  for ( int k=0; k<traces.size(); k++ ) {
    if ( ! tracefound[k] && traces[k].channel() >= PARAM_CHAN_OFFSET )
      traces[k].addErrorStr( "no matching trace found for trace " + traces[k].ident() );
  }
  return traces.failed() ? -1 : foundtraces;
}


int matchAOTraces( vector< TraceSpec > &traces )
{
#ifdef ENABLE_COMPUTATION
  modelaotraces.resize( traces.size() );
  for ( unsigned int k=0; k<modelaotraces.size(); k++ )
    modelaotraces[k] = -1;

  bool failed = false;
  string unknowntraces = "";
  int foundtraces = 0;
  for ( int j=0; j<OUTPUT_N; j++ ) {
    bool notfound = true;
    for ( unsigned int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() < PARAM_CHAN_OFFSET && traces[k].traceName() == outputNames[j] ) {
	modelaotraces[traces[k].trace()] = j;
	if ( traces[k].unit() != outputUnits[j] ) {
	  failed = true;
	  // traces[k].addErrorStr( "model output trace " + traces[k].traceName() + " requires as unit '" + outputUnits[j] + "', not '" + traces[k].unit() + "'" );
	  cerr << "model output trace " << traces[k].traceName() << " requires as unit '" << outputUnits[j] << "', not '" << traces[k].unit() << "'" << '\n';
	}
	notfound = false;
	foundtraces++;
	break;
      }
    }
    if ( notfound ) {
      unknowntraces += " ";
      unknowntraces += outputNames[j];
    }
    
  }
  if ( ! unknowntraces.empty() ) {
    //    traces.addErrorStr( "unable to match model output traces" + unknowntraces );
    cerr << "unable to match model output traces" << unknowntraces << '\n';
    failed = true;
  }

  return failed ? -1 : foundtraces;
#else
  return 0;
#endif
}


}; /* namespace dynclampmodelsim */


}; /* namespace relacs */

