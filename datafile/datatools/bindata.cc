/*
  bindata.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

/* todo: mean and variance for weighted histogram. */

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <string>
#include <float.h>
#include <cmath>
#include <getopt.h>
#include <vector>
#include <algorithm>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>
#include <relacs/array.h>
#include <relacs/stats.h>

using namespace std;
using namespace relacs;

const double noval = 1e30;
int xcol = 0; 
string xcols = "";
int ycol = -1; 
string ycols = "";
int scol = -1; 
string scols = "";
int stopempty = 100;
double bmin = noval, bmax = noval, bsize = 0.0;
int nbins = 0;
bool norm = false;
bool skipemptybins = false;
double ymin = -DBL_MAX, ymax = DBL_MAX;
bool ignorezero = true;
Str outformat = "";
TableKey binkey;
bool key = false;
bool keyonly = false;
bool numbercols = false;
bool units = true;
vector<Str> acols;
vector<int> acol; 
bool header = false;
bool dblankmode = false;
bool plotmode = false;
FILE *plt;
string binfile;
const int datacapacity = 50000;


FILE* openPlot( const string plotgeo="" )
{
  // check DISPLAY:
  char *ds = getenv( "DISPLAY" );
  if ( ds == NULL ) {
    cerr << "! no DISPLAY available!\n";
    exit( 1 );
  }

  // start gnuplot as a background process:
  string cs = "gnuplot -bg white -title bindata";
  if ( ! plotgeo.empty() ) 
    cs += " -geometry " + plotgeo;
  FILE *pltf = popen( cs.c_str(), "w" );
  // check for success:
  if ( pltf == NULL ) {
    perror( "can't open gnuplot" );
    exit( 1 );
  }

  return pltf;
}


void binData( ArrayD &xdata, ArrayD &ydata, ArrayD &sdata, int page,
	      string xunit )
{
  if ( xunit == "-" )
    xunit = "";

  if ( xdata.size() == 0 ) {
    if ( header ) {
      cout << "# index: " << page << '\n';
      cout << "#             n = " << 0 << '\n';
      if ( !plotmode )
	cout << '\n';
    }
    if ( ! plotmode ) {
      cout << "  -1  -1\n";
      cout << "\n\n";
    }
    return;
  }

  double bbmin = bmin;
  double bbmax = bmax;

  // min, max
  if ( bbmin == noval || bbmax == noval ) {
    double min = 0.0, max = 0.0;
    minMax( min, max, xdata );
    if ( bbmin == noval )
      bbmin = min;
    if ( bbmax == noval )
      bbmax = max;
  }

  // bin size:
  int nb = 10;
  double db = 1.0;
  if ( bsize > 0.0 ) {
    nb = (int)ceil( (bbmax-bbmin)/bsize );
    db = bsize;
  }
  else {
    nb = nbins;
    db = (bbmax-bbmin)/nbins;
  }

  // bins:
  double nn=0;
  ArrayD bins( nb, 0.0 );
  vector< ArrayD > bindata;
  vector< ArrayD > bindatasd;
  if ( ydata.empty() ) {
    for ( int k=0; k<xdata.size(); k++ ) {
      int inx = (int)floor( (xdata[k]-bbmin)/db );
      if ( inx>=0 && inx<nb ) {
	if ( sdata.empty() ) {
	  bins[inx]++;
	  nn += 1.0;
	}
	else {
	  double s = sdata[k];
	  double w = 1/(s*s);
	  bins[inx] += w;
	  nn += w;
	}
      }
    }
  }
  else {
    bindata.resize( nb );
    if ( ! sdata.empty() )
      bindatasd.resize( nb );
    for ( int k=0; k<xdata.size(); k++ ) {
      int inx = (int)floor( (xdata[k]-bbmin)/db );
      if ( inx>=0 && inx<nb ) {
	bindata[inx].push( ydata[k] );
	bins[inx]++;
	nn += 1.0;
	if ( ! sdata.empty() ) {
	  bindatasd[inx].push( sdata[k] );
	}
      }
    }
  }

  // save bins:
  if ( header ) {
    // basic statistics:
    double mean=0.0, stdev=0.0;
    if ( sdata.empty() || ( ! ydata.empty() ) )
      mean = meanStdev( stdev, xdata );
    else
      mean = smeanStdev( stdev, xdata, sdata );
    // more statistics:
    int n = xdata.size();
    sort( xdata.begin(), xdata.end() );

    cout << "# index: " << page << '\n';
    cout << "#             n = " << n << '\n';
    cout << "#          mean = " << mean << xunit << '\n';
    cout << "# st. deviation = " << stdev << xunit << '\n';
    cout << "#            CV = ";
    if ( fabs( mean ) > 1e-10 )
      cout << stdev/mean << '\n';
    else
      cout << "-\n";
    cout << "#        median = " << median( xdata ) << xunit << '\n';
    cout << "#   1. quartile = " << quantile( 0.25, xdata ) << xunit << '\n';
    cout << "#   3. quartile = " << quantile( 0.75, xdata ) << xunit << '\n';
    cout << "#     1. decile = " << quantile( 0.1, xdata ) << xunit << '\n';
    cout << "#     9. decile = " << quantile( 0.9, xdata ) << xunit << '\n';
    cout << "#       minimum = " << xdata[0] << xunit << '\n';
    cout << "#       maximum = " << xdata.back() << xunit << '\n';
    if ( ! plotmode )
      cout << '\n';
  }

  // key:
  if ( key && ! plotmode )
    binkey.saveKey( cout, true, numbercols, units );

  // save histogram:
  ofstream pd;
  string pdfile = "";
  if ( plt != NULL ) {
    pdfile = tmpnam( 0 );
    pd.open( pdfile.c_str() );
  }
  int pk = 0;
  for ( int k=0; k<nb; k++ ) {

    if ( bins[k] > 0.0 || ! skipemptybins ) {

      binkey.setNumber( "bin", bbmin+db*(k+0.5) );
      if ( outformat.contains( 'n' ) )
	binkey.setNumber( "n|weights", bins[k] );
      if ( outformat.contains( 'p' ) )
	binkey.setNumber( "p", bins[k]/nn );

      if ( ! ydata.empty() ) {
	double mean=0.0, stdev=0.0;
	if ( bindatasd.empty() )
	  mean = meanStdev( stdev, bindata[k] );
	else
	  mean = smeanStdev( stdev, bindata[k], bindatasd[k] );
	double sem = stdev / ::sqrt( bindata[k].size() );
	if ( outformat.contains( 'a' ) )
	  binkey.setNumber( "mean", mean );
	if ( outformat.contains( 's' ) )
	  binkey.setNumber( "s.d.", stdev );
	if ( outformat.contains( 'v' ) )
	  binkey.setNumber( "var", stdev*stdev );
	if ( outformat.contains( 'e' ) )
	  binkey.setNumber( "sem", sem );
	if ( outformat.contains( 'c' ) )
	  binkey.setNumber( "CV", fabs( mean ) > 1.0e-10 ? fabs( stdev/mean ) : 0.0 );
	sort( bindata[k].begin(), bindata[k].end() );
	int bdn = bindata[k].size();
	if ( outformat.contains( 'm' ) )
	  binkey.setNumber( "median", median( bindata[k] ) );
	if ( outformat.contains( 'q' ) ) {
	  binkey.setNumber( "1.quart", quantile( 0.25, bindata[k] ) );
	  binkey.setNumber( "3.quart", quantile( 0.75, bindata[k] ) );
	}
	if ( outformat.contains( 'd' ) ) {
	  binkey.setNumber( "1.dec", quantile( 0.1, bindata[k] ) );
	  binkey.setNumber( "9.dec", quantile( 0.9, bindata[k] ) );
	}
	if ( outformat.contains( 'x' ) ) {
	  binkey.setNumber( "min", bdn <= 0 ? 0.0 : bindata[k][0] );
	  binkey.setNumber( "max", bdn <= 0 ? 0.0 : bindata[k][bdn-1] );
	}
      }
    
      if ( !plotmode )
	binkey.saveData( cout );
      if ( plt != NULL ) {
	pd << bbmin+db*(k+0.5);
	if ( outformat.contains( 'p' ) )
	  pd << " " << bins[k]/nn << '\n';
	else
	  pd << " " << bins[k] << '\n';
      }

      pk++;
    }
  }

  // dummy output:
  if ( pk <= 0 ) {
    binkey.setNumber( "bin", -1.0 );
    if ( outformat.contains( 'n' ) )
      binkey.setNumber( "n|weights", -1.0 );
    if ( outformat.contains( 'p' ) )
      binkey.setNumber( "norm", -1.0 );
    if ( outformat.contains( 'a' ) )
      binkey.setNumber( "mean", -1.0 );
    if ( outformat.contains( 's' ) )
      binkey.setNumber( "s.d.", -1.0 );
    if ( outformat.contains( 'v' ) )
      binkey.setNumber( "var", -1.0 );
    if ( outformat.contains( 'e' ) )
      binkey.setNumber( "sem", -1.0 );
    if ( outformat.contains( 'c' ) )
      binkey.setNumber( "CV", -1.0 );
    if ( outformat.contains( 'm' ) )
      binkey.setNumber( "median", -1.0 );
    if ( outformat.contains( 'q' ) ) {
      binkey.setNumber( "1.quart", -1.0 );
      binkey.setNumber( "3.quart", -1.0 );
    }
    if ( outformat.contains( 'd' ) ) {
      binkey.setNumber( "1.dec", -1.0 );
      binkey.setNumber( "9.dec", -1.0 );
    }
    if ( outformat.contains( 'x' ) ) {
      binkey.setNumber( "min", -1.0 );
      binkey.setNumber( "max", -1.0 );
    }
    if ( !plotmode )
      binkey.saveData( cout );
    if ( plt != NULL )
      pd << "-1.0  -1.0\n";
    
  }

  if ( !plotmode )
    cout << "\n\n";

  if ( plt == NULL )
    return;

  // plot:
  pd.close();

  fprintf( plt, "set yrange [0:]\n" );
  fprintf( plt, "plot '%s' using 1:2 with boxes lt 3\n", pdfile.c_str() );
  fflush( plt );
  getchar();
  remove( pdfile.c_str() );
}


void extractUnits( const DataFile &sf, string &xunit,
		   string &yunit, string &sunit )
{
  if ( sf.newDataKey() ) {

    // find columns:
    if ( !xcols.empty() ) {
      int c = sf.column( xcols );
      if ( c >= 0 )
	xcol = c;
    }
    if ( !ycols.empty() ) {
      int c = sf.column( ycols );
      if ( c >= 0 )
	ycol = c;
    }
    if ( !scols.empty() ) {
      int c = sf.column( scols );
      if ( c >= 0 )
	scol = c;
    }

    // set units:
    xunit = sf.key().unit( xcol );
    if ( xunit.empty() )
      xunit = "-";
    if ( plt != NULL )
      fprintf( plt, "set xlabel '%s [%s]'\n", 
	       sf.key().ident( xcol ).c_str(), xunit.c_str() );
    
    yunit = sf.key().unit( ycol );
    if ( yunit.empty() )
      yunit = "-";

    sunit = sf.key().unit( scol );
    if ( sunit.empty() )
      sunit = "-";

  }
}


void readData( DataFile &sf )
{
  // read meta data and key:
  sf.readMetaData();

  // get columns and units:
  string xunit = "-";
  string yunit = "-";
  string sunit = "-";
  if ( sf.good() )
    extractUnits( sf, xunit, yunit, sunit );

  binkey.addNumber( "bin", xunit, "%10.4g" );

  if ( ycol < 0 && ycols.empty() ) {
    if ( outformat.empty() )
      outformat = norm ? "p" : "n";
    for ( int k=0; k<outformat.size(); k++ ) {
      switch ( outformat[k] ) {
      case 'n':
	if ( scol < 0 )
	  binkey.addNumber( "n", "1", "%5.0f" );
	else
	  binkey.addNumber( "weights", "1/"+sunit+"^2", "%5.3f" );
	break;
      case 'p':
	binkey.addNumber( "p", "1", "%5.3f" );
	break;
      case 'a':
      case 's':
      case 'e':
      case 'v':
      case 'c':
      case 'm':
      case 'q':
      case 'd':
      case 'x':
	break;
      default:
	cerr << "! warning: unknown output column " << outformat[k] << " !\n";
      }
    }
  }
  else {
    if ( outformat.empty() )
      outformat = norm ? "pas" : "nas";
    for ( int k=0; k<outformat.size(); k++ ) {
      switch ( outformat[k] ) {
      case 'n':
	binkey.addNumber( "n", "1", "%5.0f" );
	break;
      case 'p':
	binkey.addNumber( "norm", "1", "%5.3f" );
	break;
      case 'a':
	binkey.addNumber( "mean", yunit, "%10.4g" );
	break;
      case 's':
	binkey.addNumber( "s.d.", yunit, "%10.4g" );
	break;
      case 'e':
	binkey.addNumber( "sem", yunit, "%10.4g" );
	break;
      case 'v':
	binkey.addNumber( "var", yunit+"^2", "%10.4g" );
	break;
      case 'c':
	binkey.addNumber( "CV", "1", "%10.4g" );
	break;
      case 'm':
	binkey.addNumber( "median", yunit, "%10.4g" );
	break;
      case 'q':
	binkey.addNumber( "1.quart", yunit, "%10.4g" );
	binkey.addNumber( "3.quart", yunit, "%10.4g" );
	break;
      case 'd':
	binkey.addNumber( "1.dec", yunit, "%10.4g" );
	binkey.addNumber( "9.dec", yunit, "%10.4g" );
	break;
      case 'x':
	binkey.addNumber( "min", yunit, "%10.4g" );
	binkey.addNumber( "max", yunit, "%10.4g" );
	break;
      default:
	cerr << "! warning: unknown output column " << outformat[k] << " !\n";
      }
    }
  }
  numbercols = ( binkey.columns() > 3 );

  vector<Parameter*> aparam;
  aparam.reserve( acols.size() );
  vector<int> amode; 
  amode.reserve( acols.size() ); // 0: column number, 1: column title, 2: meta data
  for ( unsigned int k=0; k<acols.size(); k++ ) {
    aparam.push_back( &binkey[acol[k]] );
    amode.push_back( 0 );
    acol[k] = int( acols[k].number( -1.0 ) );
    if ( acol[k] <= 0 )
      amode[k] = 1;
    unsigned int i = acols[k].rfind( '>' );
    if ( i != string::npos )
      i++;
    else {
      i=0;
      if ( acols[k].size() > 1 && acols[k][0] == '_' )
	i=1;
      else if ( acols[k].size() > 2 && acols[k][1] == '_' )
	i=2;
      if ( i > 0 )
	amode[k] = 2;
    }
    if ( amode[k] == 0 && acol[k] > 0 )
      acol[k] -= 1;
    aparam[k]->setIdent( acols[k].substr( i ) );
  }

  if ( keyonly ) {
    binkey.saveKey( cout, true, numbercols, units );
    return;
  }

  int page = 0;
  while ( sf.good() ) {

    // meta data:
    for ( int l=0; l<sf.levels(); l++ ) {
      Options opt;
      for ( unsigned int k=0; k<acols.size(); k++ ) {
	if ( amode[k] == 2 && ( acol[k] == l || acol[k] < 0 ) ) {
	  if ( opt.empty() )
	    opt.load( sf.metaData( l ).strippedComments( "-#" ) );
	  Options::iterator p = opt.find( aparam[k]->ident() );
	  if ( p != opt.end() ) {
	    aparam[k]->setNumber( p->number() );
	    aparam[k]->setUnit( p->unit() );
	    amode[k] = 3;
	  }
	}
      }
    }
    for ( unsigned int k=0; k<amode.size(); k++ )
      if ( amode[k] == 3 )
	amode[k] = 2;

    // find data column:
    if ( sf.newDataKey() ) {
      if ( !acols.empty() ) {
	for ( unsigned int k=0; k<acols.size(); k++ ) {
	  if ( amode[k] <= 1 ) {
	    if ( amode[k] == 1 ) {
	      int c = sf.column( acols[k] );
	      if ( c >= 0 )
		acol[k] = c;
	    }
	    string ts = sf.key().ident( acol[k] );
	    if ( !ts.empty() ) {
	      aparam[k]->setIdent( ts );
	    }
	  }
	}
      }
    }

    // read data:
    sf.initData();
    ArrayD xdata;
    xdata.clear();
    xdata.reserve( datacapacity );
    ArrayD ydata;
    ydata.clear();
    if ( ycol >= 0 )
      ydata.reserve( datacapacity );
    ArrayD sdata;
    sdata.clear();
    if ( scol >= 0 )
      sdata.reserve( datacapacity );
    Str space( dblankmode ? Str::DoubleWhiteSpace : Str::WhiteSpace );
    do {
      int index = 0;
      int word = 0;
      Str line = sf.line();
      double xval = 0.0;
      double yval = 0.0;
      double sval = 1.0;
      for ( int k=0; index>=0; k++ ) {
	word = line.nextWord( index, space, sf.comment() );
	if ( word >= 0 ) {
	  for ( int c=0; c<(int)acols.size(); c++ ) {
	    if ( amode[c] <= 1 && k == acol[c] )
	      aparam[c]->setNumber( line.number( -1.0, word ) );
	  }
	  if ( k == xcol )
	    xval = line.number( -1.0, word );
	  if ( k == ycol )
	    yval = line.number( -1.0, word );
	  if ( k == scol )
	    sval = line.number( -1.0, word );
	}
      }
      if ( ( ycol < 0 || ( yval > ymin && yval < ymax ) ) && 
	   ! ( ignorezero && sval <= 0.0 ) ) {
	if ( xdata.size() == xdata.capacity() ) {
	  xdata.reserve( xdata.capacity() + datacapacity );
	  if ( ycol >= 0 )
	    ydata.reserve( ydata.capacity() + datacapacity );
	  if ( scol >= 0 )
	    sdata.reserve( sdata.capacity() + datacapacity );
	}
	xdata.push( xval );
	if ( ycol >= 0 )
	  ydata.push( yval );
	if ( scol >= 0 )
	  sdata.push( sval );
      }
    } while ( sf.readDataLine( stopempty ) );

    binData( xdata, ydata, sdata, page, xunit );

    page++;

    sf.readMetaData();

  }
  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "bindata [-d ###] [-D] [-c xxx] [-y yyy [-s sss]] [-l ###] [-u ###]\n";
  cerr << "        [-b ###] [-w ###] [-n] [-S] [-e ###] [-E ###] [-z] [-f ###]\n";
  cerr << "        [-k|-K [-U]] [-a xxx] [-m] [-p] [-o xxx] fname\n";
  cerr << '\n';
  cerr << "makes a frequency histogram from one column in data file <fname>.\n";
  cerr << "-c, -x: ### specifies x column (default is first column).\n";
  cerr << "    the column can be either a number (1,2,...)\n";
  cerr << "    or a string containing the column title.\n";
  cerr << "-y: ### specifies y column (default: no y-column).\n";
  cerr << "-s: ### specifies column containing weights as standard deviations\n";
  cerr << "    (default: no weights).\n";
  cerr << "-l, -u: lower and upper bound of histogram\n";
  cerr << "        default is minimum and maximum of data (two passes!)\n";
  cerr << "-b: number of bins (default 10), or\n";
  cerr << "-w: width of the bins\n";
  cerr << "-n: normalize to frequency probabilities (has only an effect if no\n";
  cerr << "    output format is specified with -f).\n";
  cerr << "-S: Skip empty bins, i.e. do not print results for empty bins.\n";
  cerr << "-e: exclude y-column values less than or equal to ###.\n";
  cerr << "-E: exclude y-column values greater than or equal to ###.\n";
  cerr << "-z: discard s-column values less than or equal to zero.\n";
  cerr << "-f: select output columns...\n";
  cerr << "    n: number of data points in bin\n";
  cerr << "    p: normalized number of data points in bin to frequency probabilities\n";
  cerr << "    for two variables x and y:\n";
  cerr << "    a: average of y-data in bin\n";
  cerr << "    s: standard deviation\n";
  cerr << "    e: standard error mean\n";
  cerr << "    v: variance\n";
  cerr << "    c: coefficient of variation\n";
  cerr << "    m: median\n";
  cerr << "    q: 1. and 3. quartile\n";
  cerr << "    d: 1. and 9. decile\n";
  cerr << "    x: minimum and maximum\n";
  cerr << "-k: add key to the output table\n";
  cerr << "-K: just print the key, don't process data\n";
  cerr << "-U: don't print the line with the units in the key\n";
  cerr << "-a: value that is added to the table (either column number, column title\n";
  cerr << "    (takes the value from the last line), 'ident:value' (value can be a\n";
  cerr << "    number with an unit or a string), or 'l_ident' (take value of ident\n";
  cerr << "    in meta data level l, l can be ommited))\n";
  cerr << "-m: add header with the statistics to the histogram data.\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-D: more than one space between data columns required.\n";
  cerr << "-p: plot histogram on screen.\n";
  cerr << "-o: write binned data into file ### instead to standard out\n";
  cerr << '\n';
  cerr << "x: simply count values of x-column into bins\n";
  cerr << "x s: weight values of x-column with 1/s^2\n";
  cerr << "x y: calculate mean and standard deviation of y values for each bin of x-values\n";
  cerr << "x y s: calculate mean and standard deviation of y values weighted with\n";
  cerr << "       1/s^2 for each bin of x-values\n";
  cerr << '\n';
  exit( 1 );
}


void readArgs( int argc, char *argv[], int &filec )
{
  int c;

  if ( argc <= 1 )
    WriteUsage();
  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "d:c:x:y:s:o:l:u:b:w:nSe:E:zf:kKUa:mDqp" )) >= 0 ) {
    switch ( c ) {
    case 'x':
    case 'c':
      if ( optarg != NULL ) {
	if ( sscanf( optarg, "%d", &xcol ) == 0 )
	  xcols = optarg;
	else 
	  xcol--;
      }
      break;
    case 'y':
      if ( optarg != NULL ) {
	if ( sscanf( optarg, "%d", &ycol ) == 0 )
	  ycols = optarg;
	else 
	  ycol--;
      }
      break;
    case 's':
      if ( optarg != NULL ) {
	if ( sscanf( optarg, "%d", &scol ) == 0 )
	  scols = optarg;
	else 
	  scol--;
      }
      break;
    case 'l':
      if ( optarg == NULL || sscanf( optarg, "%lf", &bmin ) == 0 )
	bmin = noval;
      break;
    case 'u':
      if ( optarg == NULL || sscanf( optarg, "%lf", &bmax ) == 0 )
	bmax = noval;
      break;
    case 'w':
      if ( optarg == NULL || sscanf( optarg, "%lf", &bsize ) == 0 )
	bsize = 0.0;
      else
	nbins = 0;
      break;
    case 'b':
      if ( optarg == NULL || sscanf( optarg, "%d", &nbins ) == 0 )
	nbins = 10;
      else
	bsize = 0;
      break;
    case 'n':
      norm = true;
      break;
    case 'S':
      skipemptybins = true;
      break;
    case 'e':
      if ( optarg != NULL ) {
	double v = 0.0;
	if ( sscanf( optarg, "%lf", &v ) > 0 )
	  ymin = v;
      }
      break;
    case 'E':
      if ( optarg != NULL ) {
	double v = 0.0;
	if ( sscanf( optarg, "%lf", &v ) > 0 )
	  ymax = v;
      }
      break;
    case 'z':
      ignorezero = true;
      break;
    case 'f':
      if ( optarg != NULL )
	outformat = optarg;
      break;
    case 'k':
      key = true;
      break;
    case 'K':
      keyonly = true;
      break;
    case 'U':
      units = false;
      break;
    case 'a':
      if ( optarg != NULL ) {
	Str as = optarg;
	string aident = as.ident();
	Str val = as.value();
	if ( ! aident.empty() && ! val.empty() ) {
	  double e = 0.0;
	  string aunit = "";
	  double aval = val.number( e, aunit, MAXDOUBLE );
	  if ( aval == MAXDOUBLE ) {
	    binkey.addText( aident );
	    binkey.setText( aident, val );
	  }
	  else {
	    binkey.addNumber( aident, aunit, "%7.5g" );
	    binkey.setNumber( aident, aval );
	  }
	}
	else {
	  acols.push_back( as );
	  acol.push_back( binkey.columns() );
	  binkey.addNumber( as, "-", "%7.5g" );
	}
      }
      break;
    case 'm':
      header = true;
      break;
    case 'd':
      if ( optarg == NULL ||
	   sscanf( optarg, "%d", &stopempty ) == 0 ||
	   stopempty < 1 )
	stopempty = 1;
      break;
    case 'D':
      dblankmode = true;
      break;
    case 'o':
      if ( optarg != NULL )
	binfile = optarg;
      break;
    case 'p':
      plotmode = true;
      break;
    default :
      WriteUsage();
    }
  }
  if ( optind < argc && argv[optind][0] == '?' )
    WriteUsage();
  filec = optind;
  if ( bmin < noval && bmax < noval && bmin > bmax ) {
    double swap = bmin;
    bmin = bmax;
    bmax = swap;
  }
  if ( bsize == 0.0 && nbins == 0 )
    nbins = 10;
}


int main( int argc, char *argv[] )
{
  acols.reserve( 10 );
  acol.reserve( 10 );
  int filec = 0;
  readArgs( argc, argv, filec );

  // redirect cin:
  DataFile sf;
  if ( ! keyonly ) {
    if ( argc > filec ) {
      sf.open( argv[filec] );
      if ( !sf.good() ) {
	cerr << "! can't open file " << argv[filec] << " for reading\n";
	return 1;
      }
      filec++;
    }
    else
      sf.open( cin );
  }

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( plotmode )
    binfile = "";
  if ( !binfile.empty() ) {
    df.open( binfile.c_str() );
    if ( !df.good() ) {
      cerr << "! can't open file " << binfile << " for writing\n";
      return 1;
    }
    coutb = cout.rdbuf();
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }

  if ( plotmode )
    plt = openPlot();
  else 
    plt = 0;

  readData( sf );

  if ( plt != NULL )
    pclose( plt );

  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
