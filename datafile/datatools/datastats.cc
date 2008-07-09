/*
  datastats.cc
  

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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <cmath>
#include <getopt.h>
#include <vector>
#include <algorithm>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>
#include <relacs/array.h>
#include <relacs/stats.h>
#include <relacs/statstests.h>

using namespace std;
using namespace relacs;


const double noval = 1e30;
vector<int> xcol; 
vector<string> xcols;
string xunit = "-";
int ycol = -1; 
string ycols = "";
string yunit = "-";
int scol = -1; 
string scols = "";
double xmin = -DBL_MAX, xmax = DBL_MAX;
bool ignorezero = true;
int minn = 1;
vector<Str> acols;
vector<int> acol; 
int stopempty = 100;
Str outformat = "";
double threshold = 0.0;
bool key = false;
bool keyonly = false;
bool numbercols = false;
bool dblankmode = false;
bool datamode = true;
string statsfile = "";
TableKey statskey;
const int datacapacity = 50000;


void analyseData( ArrayD &data, ArrayD &sig, int page, TableKey &statskey )
{
  if ( data.size() == 0 )
    return;

  double mean = 0.0;
  double stdev = 0.0;
  if ( sig.size() == data.size() )
    mean = smeanStdev( stdev, data, sig );
  else
    mean = data.mean( stdev );
  double sem = stdev / ::sqrt( data.size() );

  if ( outformat.contains( 'a' ) )
    statskey.setNumber( "mean", mean ).setUnit( xunit );
  if ( outformat.contains( 's' ) )
    statskey.setNumber( "s.d.", stdev ).setUnit( xunit );
  if ( outformat.contains( 'v' ) )
    statskey.setNumber( "var", stdev*stdev ).setUnit( xunit + "^2" );
  if ( outformat.contains( 'e' ) )
    statskey.setNumber( "sem", sem ).setUnit( xunit );
  if ( outformat.contains( 'c' ) )
    statskey.setNumber( "CV", fabs( mean ) > 1.0e-10 ? fabs( stdev/mean ) : 0.0 );
  if ( outformat.contains( 'z' ) )
    statskey.setNumber( "sum", sum( data ) ).setUnit( xunit );

  // t-Test:
  if ( outformat.contains( 't' ) ) {
    double t = ::sqrt( data.size() ) * ::fabs( mean - threshold ) / stdev;
    int df = data.size() - 2;
    double p = incBeta( 0.5*df, 0.5, df/(df+t*t) );
    statskey.setNumber( "t-Test>t", df >= 0 ? t : -1.0 );
    statskey.setNumber( "t-Test>p", df >= 0 ? p : -1.0 );
  }

  // Sign-Test:
  if ( outformat.contains( 'S' ) ) {
    int sn = 0;
    double p = 0.0;
    int tail = 0;
    if ( outformat.contains( "S+" ) )
      tail = 1;
    else if ( outformat.contains( "S-" ) )
      tail = -1;
    signTest( data, threshold, tail, sn, p );
    statskey.setNumber( "Sign-Test>n", sn );
    statskey.setNumber( "Sign-Test>p", p );
  }

  sort( data.begin(), data.end() );

  ArrayD::const_iterator lb = lower_bound( data.begin(), data.end(), threshold );
  int lt = lb - data.begin();
  ArrayD::const_iterator ub = upper_bound( data.begin(), data.end(), threshold );
  int gt = data.end() - ub;
 
  if ( outformat.contains( 'm' ) )
    statskey.setNumber( "median", median( data ) ).setUnit( xunit );
  if ( outformat.contains( 'q' ) ) {
    statskey.setNumber( "1.quart", quantile( 0.25, data ) ).setUnit( xunit );
    statskey.setNumber( "3.quart", quantile( 0.75, data ) ).setUnit( xunit );
  }
  if ( outformat.contains( 'd' ) ) {
    statskey.setNumber( "1.dec", quantile( 0.1, data ) ).setUnit( xunit );
    statskey.setNumber( "9.dec", quantile( 0.9, data ) ).setUnit( xunit );
  }
  if ( outformat.contains( 'x' ) ) {
    statskey.setNumber( "min", data[0] ).setUnit( xunit );
    statskey.setNumber( "max", data[data.size()-1] ).setUnit( xunit );
  }
  if ( outformat.contains( 'w' ) )
    statskey.setNumber( "width", data[data.size()-1] - data[0] ).setUnit( xunit );
  if ( outformat.contains( '<' ) || outformat.contains( '-' ) )
    statskey.setInteger( "less", lt );
  if ( outformat.contains( '>' ) || outformat.contains( '+' ) )
    statskey.setInteger( "more", gt );
  if ( outformat.contains( 'n' ) )
    statskey.setNumber( "n>n", double( data.size() ) ); 

  if ( key ) {
    statskey.saveKey( cout, true, numbercols );
    key = false;
  }
  statskey.saveData( cout );

  if ( datamode ) {
    statskey.saveMetaData( cerr );
  }
}


struct rankdata
{
  rankdata( void ) : X(0.0), Id(0) {};
  rankdata( double x, int id ) : X(x), Id(id) {};
  rankdata( const rankdata &r ) : X(r.X), Id(r.Id) {};
  rankdata &operator=( int v ) { X = v; return *this; };
  rankdata &operator=( double v ) { X = v; return *this; };
  bool operator<( const rankdata &r ) const { return ( X < r.X ); };
  bool operator==( const rankdata &r ) const { return ( X == r.X ); };
  bool operator!=( const rankdata &r ) const { return ( X != r.X ); };
  double X;
  int Id;
};


void analyseCor( ArrayD &xdata, ArrayD &ydata, ArrayD &sig, int page, TableKey &statskey )
{
  if ( xdata.size() <= 1 || ydata.size() <= 1 )
    return;

  // n:
  int n = xdata.size();

  // regression:
  double b, bu, m, mu, chisq;
  if ( sig.size() == ydata.size() )
    lineFit( xdata, ydata, sig, b, bu, m, mu, chisq );
  else
    lineFit( xdata, ydata, b, bu, m, mu, chisq );

  if ( outformat.contains( 'b' ) ) {
    statskey.setNumber( "linear regression>b", b ).setUnit( yunit );
    statskey.setNumber( "linear regression>bsd", bu ).setUnit( yunit );
  }
  if ( outformat.contains( 'm' ) ) {
    statskey.setNumber( "linear regression>m", m ).setUnit( yunit + "/" + xunit );
    statskey.setNumber( "linear regression>msd", mu ).setUnit( yunit + "/" + xunit );
  }

  // correlation:
  double r = 0.0;
  if ( sig.size() == ydata.size() )
    r = scorrCoef( xdata, ydata, sig );
  else
    r = corrCoef( xdata, ydata );

  const double tiny = 1.0e-20;
  double rz = 0.5*log( (1.0 + r + tiny) / (1.0 - r + tiny) );
  double rp = pearsonTest( r, xdata.size() );

  if ( outformat.contains( 'r' ) ) {
    statskey.setNumber( "correlation>r", r );
    statskey.setNumber( "correlation>rsq", r*r );
    statskey.setNumber( "correlation>p", rp );
  }
  if ( outformat.contains( 'z' ) )
    statskey.setNumber( "correlation>z", rz );

  double ax = mean( xdata );  
  double ay = mean( ydata );
  double vx = variance( ax, xdata );
  double vy = variance( ay, ydata );

  // unpaired t-Test:
  if ( outformat.contains( 'u' ) ) {
    int udf = 2*n - 2;
    double svar = (vx+vy)*n/udf;
    double ut = (ax-ay)/::sqrt( svar*(2.0/n) );
    double up = incBeta( 0.5*udf, 0.5, udf/(udf+ut*ut) );
    statskey.setNumber( "unpaired t-Test>t", ut );
    statskey.setNumber( "unpaired t-Test>p", up );
  }

  // paired t-Test:
  if ( outformat.contains( 'p' ) ) {
    double cova = cov( xdata, ydata );
    double sd = ::sqrt( ( vx + vy - 2.0*cova ) / n );
    double pt = ( ax - ay ) / sd;
    int pdf = n-1;
    double pp = incBeta( 0.5*pdf, 0.5, pdf/(pdf+pt*pt) );
    statskey.setNumber( "paired t-Test>t", pt );
    statskey.setNumber( "paired t-Test>p", pp );
  }

  // U-Test:
  if ( outformat.contains( 'U' ) ) {
    int n1 = xdata.size();
    int n2 = ydata.size();
    // workspace:
    vector<rankdata> rdata;
    rdata.reserve( n1 + n2 );
    for ( int k=0; k<n1; k++ )
      rdata.push_back( rankdata( xdata[k], 0 ) );
    for ( int k=0; k<n2; k++ )
      rdata.push_back( rankdata( ydata[k], 1 ) );
    // rank:
    sort( rdata.begin(), rdata.end() );
    rank( rdata );
    // sum of ranks:
    double R1 = 0.0;
    double R2 = 0.0;
    for ( unsigned int k=0; k<rdata.size(); k++ )
      if ( rdata[k].Id == 0 )
	R1 += rdata[k].X;
      else
	R2 += rdata[k].X;
    // U:
    double U1 = n1*n2 + 0.5*n1*(n1+1.0) - R1;
    double U2 = n1*n2 + 0.5*n2*(n2+1.0) - R2;
    double U = U1 < U2 ? U1 : U2;
    double z = (R1 - n1 * (n1 + n2 + 1.0) / 2.0) / ::sqrt (n1 * n2 * (n1 + n2 + 1.0) / 12.0);
    double cdf = 0.5 * (1.0 + ::erf( z / ::sqrt( 2.0 ) ) );
    double p = 0.0;
    if ( outformat.contains( "U+" ) )
      p = cdf;
    else if ( outformat.contains( "U-" ) )
      p = 1.0 - cdf;
    else
      p = 2.0 * std::min( cdf, 1.0 - cdf );
    statskey.setNumber( "Mann-Whitney U-Test>U", U );
    statskey.setNumber( "Mann-Whitney U-Test>p", p );
  }

  // Wilcoxon-Test:
  if ( outformat.contains( 'W' ) ) {
    double W = 0.0;
    double p = 0.0;
    int tail = 0;
    if ( outformat.contains( "W+" ) )
      tail = 1;
    else if ( outformat.contains( "W-" ) )
      tail = -1;
    wilcoxonTest( xdata, ydata, tail, W, p );
    statskey.setNumber( "Wilcoxon-Test>W", W );
    statskey.setNumber( "Wilcoxon-Test>p", p );
  }

  // F-Test:
  if ( outformat.contains( 'F' ) ) {
    double F = 0.0;
    double df1 = 0.0;
    double df2 = 0.0;
    if ( vx > vy ) {
      F = vx / vy;
      df1 = xdata.size() - 1;
      df2 = ydata.size() - 1;
    }
    else {
      F = vy / vx;
      df1 = ydata.size() - 1;
      df2 = xdata.size() - 1;
    }
    double Fp = 2.0*incBeta( 0.5*df2, 0.5*df1, df2/(df2+df1*F) );
    if ( Fp > 1.0 )
      Fp = 2.0 - Fp;
    statskey.setNumber( "F-Test>F", F );
    statskey.setNumber( "F-Test>p", Fp );
  }

  // kendall's tau:
  if ( outformat.contains( 'k' ) ) {
    unsigned long n1=0;
    unsigned long n2=0;
    long is=0;
    for ( int j=0; j<n-1; j++ ) {
      for ( int k=(j+1); k<n; k++ ) {
	double a1 = xdata[j] - xdata[k];
	double a2 = ydata[j] - ydata[k];
	double aa = a1*a2;
	if ( aa ) {
	  ++n1;
	  ++n2;
	  aa > 0.0 ? ++is : --is;
	} 
	else {
	  if ( a1 )
	    ++n1;
	  if ( a2 )
	    ++n2;
	}
      }
    }
    double tau = is / ( sqrt( (double)n1 ) * sqrt( (double)n2 ) );
    double svar = ( 4.0 * n + 10.0 ) / ( 9.0 * n * ( n - 1.0 ) );
    double z = tau / sqrt( svar );
    double taup = ::erfc( fabs( z ) / 1.4142136 );
    statskey.setNumber( "Kendall's tau>tau", tau );
    statskey.setNumber( "Kendall's tau>p", taup );
  }

  // mean difference:
  if ( outformat.contains( 'd' ) ) {
    ArrayD diff = ydata - xdata;
    double dmean = diff.mean();
    double dsd = diff.stdev( dmean );
    statskey.setNumber( "Difference>d", dmean ).setUnit( xunit );
    statskey.setNumber( "Difference>s.d.", dsd ).setUnit( xunit );
  }

  if ( outformat.contains( 'n' ) )
    statskey.setNumber( "n>n", double( n ) ); 

  if ( key ) {
    statskey.saveKey( cout, true, numbercols );
    key = false;
  }

  statskey.saveData( cout );

  if ( datamode ) {
    statskey.saveMetaData( cerr );
  }
}


void readData( DataFile &sf )
{
  if ( ycol < 0 && ycols.empty() ) {
    if ( outformat.empty() )
      outformat = "asevcmqdx<>ntS";
    bool momentslabel = false;
    bool quantileslabel = false;
    bool threshlabel = false;
    for ( int k=0; k<outformat.size(); k++ ) {
      switch ( outformat[k] ) {
      case 'a':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "mean", "-", "%10.4g" );
	break;
      case 's':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "s.d.", "-", "%10.4g" );
	break;
      case 'e':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "sem", "-", "%10.4g" );
	break;
      case 'v':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "var", "-", "%10.4g" );
	break;
      case 'c':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "CV", "1", "%10.4g" );
	break;
      case 'z':
	if ( ! momentslabel ) {
	  statskey.addLabel( "moments" );
	  momentslabel = true;
	}
	statskey.addNumber( "sum", "-", "%10.4g" );
	break;
      case 'm':
	if ( ! quantileslabel ) {
	  statskey.addLabel( "quantiles" );
	  quantileslabel = true;
	}
	statskey.addNumber( "median", "-", "%10.4g" );
	break;
      case 'q':
	if ( ! quantileslabel ) {
	  statskey.addLabel( "quantiles" );
	  quantileslabel = true;
	}
	statskey.addNumber( "1.quart", "-", "%10.4g" );
	statskey.addNumber( "3.quart", "-", "%10.4g" );
	break;
      case 'd':
	if ( ! quantileslabel ) {
	  statskey.addLabel( "quantiles" );
	  quantileslabel = true;
	}
	statskey.addNumber( "1.dec", "-", "%10.4g" );
	statskey.addNumber( "9.dec", "-", "%10.4g" );
	break;
      case 'x':
	if ( ! quantileslabel ) {
	  statskey.addLabel( "quantiles" );
	  quantileslabel = true;
	}
	statskey.addNumber( "min", "-", "%10.4g" );
	statskey.addNumber( "max", "-", "%10.4g" );
	break;
      case 'w':
	if ( ! quantileslabel ) {
	  statskey.addLabel( "quantiles" );
	  quantileslabel = true;
	}
	statskey.addNumber( "width", "-", "%10.4g" );
	break;
      case '<':
      case '-':
	if ( ! threshlabel ) {
	  statskey.addLabel( "t=" + Str( threshold ) );
	  threshlabel = true;
	}
	statskey.addNumber( "less", "1", "%5.0f" );
	break;
      case '>':
      case '+':
	if ( ! threshlabel ) {
	  statskey.addLabel( "t=" + Str( threshold ) );
	  threshlabel = true;
	}
	statskey.addNumber( "more", "1", "%5.0f" );
	break;
      case 'n':
	statskey.addLabel( "n" );
	statskey.addNumber( "n", "1", "%5.0f" );
	break;
      case 't':
	statskey.addLabel( "t-Test" );
	statskey.addNumber( "t", "1", "%10.4g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'S':
	statskey.addLabel( "Sign-Test" );
	statskey.addNumber( "n", "1", "%6.0f" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'b':
      case 'r':
      case 'p': 
      case 'u':
      case 'U':
      case 'W':
      case 'F':
      case 'k':
	break;
      default:
	cerr << "! warning: unknown output column " << outformat[k] << " !\n";
      }
    }
  }
  else {
    if ( outformat.empty() )
      outformat = "mbrzupUFkdn";
    bool linearlabel = false;
    bool rlabel = false;
    for ( int k=0; k<outformat.size(); k++ ) {
      switch ( outformat[k] ) {
      case 'm':
	if ( ! linearlabel ) {
	  statskey.addLabel( "linear regression" );
	  linearlabel = true;
	}
	statskey.addNumber( "m", "-", "%10.4g" );
	statskey.addNumber( "sd", "-", "%10.4g" );
	break;
      case 'b':
	if ( ! linearlabel ) {
	  statskey.addLabel( "linear regression" );
	  linearlabel = true;
	}
	statskey.addNumber( "b", "-", "%10.4g" );
	statskey.addNumber( "sd", "-", "%10.4g" );
	break;
      case 'r':
	if ( ! rlabel ) {
	  statskey.addLabel( "correlation" );
	  rlabel = true;
	}
	statskey.addNumber( "r", "1", "%6.3g" );
	statskey.addNumber( "rsq", "1", "%6.3g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'z':
	if ( ! rlabel ) {
	  statskey.addLabel( "correlation" );
	  rlabel = true;
	}
	statskey.addNumber( "z", "1", "%10.4g" );
	break;
      case 'u':
	statskey.addLabel( "unpaired t-Test" );
	statskey.addNumber( "t", "1", "%6.3g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'p':
	statskey.addLabel( "paired t-Test" );
	statskey.addNumber( "t", "1", "%6.3g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'U':
	statskey.addLabel( "Mann-Whitney U-Test" );
	statskey.addNumber( "U", "1", "%7.1f" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'W':
	statskey.addLabel( "Wilcoxon-Test" );
	statskey.addNumber( "W", "1", "%7.1f" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'F':
	statskey.addLabel( "F-Test" );
	statskey.addNumber( "F", "1", "%6.3g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'k':
	statskey.addLabel( "Kendall's tau" );
	statskey.addNumber( "tau", "1", "%6.3g" );
	statskey.addNumber( "p", "1", "%7.5f" );
	break;
      case 'd':
	statskey.addLabel( "Difference" );
	statskey.addNumber( "d", "-", "%10.4g" );
	statskey.addNumber( "s.d.", "-", "%7.5f" );
	break;
      case 'n':
	statskey.addLabel( "n" );
	statskey.addNumber( "n", "1", "%5.0f" );
	break;
      case '+':
      case '-':
      case 'a':
      case 's':
      case 'e':
      case 'v':
      case 'c':
      case 'q':
      case '<':
      case '>':
      case 't':
      case 'S':
	break;
      default:
	cerr << "! warning: unknown output column " << outformat[k] << " !\n";
      }
    }
  }

  vector<Parameter*> aparam;
  aparam.reserve( acols.size() );
  vector<int> amode; 
  amode.reserve( acols.size() ); // 0: column number, 1: column title, 2: meta data
  for ( unsigned int k=0; k<acols.size(); k++ ) {
    aparam.push_back( &statskey[acol[k]] );
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

  if ( key && keyonly ) {
    statskey.saveKey( cout, true, numbercols );
    key = false;
    return;
  }

  int page = 0;

  sf.readMetaData();

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
      TableKey tk;
      tk.loadKey( sf.dataKey() );
      // find columns:
      for ( unsigned int k=0; k<xcols.size(); k++ ) {
	if ( !xcols[k].empty() ) {
	  int c = tk.column( xcols[k] );
	  if ( c >= 0 )
	    xcol[k] = c;
	}
      }
      if ( !ycols.empty() ) {
	int c = tk.column( ycols );
	if ( c >= 0 )
	  ycol = c;
      }
      if ( !scols.empty() ) {
	int c = tk.column( scols );
	if ( c >= 0 )
	  scol = c;
      }
      if ( !acols.empty() ) {
	for ( unsigned int k=0; k<acols.size(); k++ ) {
	  if ( amode[k] <= 1 ) {
	    if ( amode[k] == 1 ) {
	      int c = tk.column( acols[k] );
	      if ( c >= 0 )
		acol[k] = c;
	    }
	    string ts = tk.ident( acol[k] );
	    if ( !ts.empty() ) {
	      aparam[k]->setIdent( ts );
	    }
	  }
	}
      }
      // set units:
      xunit = tk.unit( xcol[0] );
      if ( xunit.empty() )
	xunit = "-";
      yunit = tk.unit( ycol );
      if ( yunit.empty() )
	yunit = "-";
    }

    // read data:
    sf.initData();
    ArrayD xdata;
    xdata.reserve( datacapacity );
    ArrayD x2data;
    if ( xcol.size() > 1 )
      x2data.reserve( datacapacity );
    ArrayD ydata;
    if ( ycol >= 0 )
      ydata.reserve( datacapacity );
    ArrayD sdata;
    if ( scol >= 0 )
      sdata.reserve( datacapacity );
    Str space( dblankmode ? Str::DoubleWhiteSpace : Str::WhiteSpace );
    do {
      int index = 0;
      int word = 0;
      Str line = sf.line();
      double xval = 0.0;
      double x2val = 0.0;
      double yval = 0.0;
      double sval = 1.0;
      for ( int k=0; index>=0; k++ ) {
	word = line.nextWord( index, space, sf.comment() );
	if ( word >= 0 ) {
	  for ( int c=0; c<(int)acols.size(); c++ ) {
	    if ( amode[c] <= 1 && k == acol[c] )
	      aparam[c]->setNumber( line.number( -1.0, word ) );
	  }
	  if ( k == xcol[0] )
	    xval = line.number( -1.0, word );
	  if ( xcol.size() > 1 && k == xcol[1] )
	    x2val = line.number( -1.0, word );
	  if ( k == ycol )
	    yval = line.number( -1.0, word );
	  if ( k == scol )
	    sval = line.number( -1.0, word );
	}
      }
      if ( xval > xmin && xval < xmax &&
	   ! ( ignorezero && sval <= 0.0 ) ) {
	if ( xdata.size() == xdata.capacity() ) {
	  xdata.reserve( xdata.capacity() + datacapacity );
	  if ( xcol.size() > 1 )
	    x2data.reserve( x2data.capacity() + datacapacity );
	  if ( ycol >= 0 )
	    ydata.reserve( ydata.capacity() + datacapacity );
	  if ( scol >= 0 )
	    sdata.reserve( sdata.capacity() + datacapacity );
	}
	xdata.push( xval );
	if ( xcol.size() > 1 )
	  x2data.push( x2val );
	if ( ycol >= 0 )
	  ydata.push( yval );
	if ( scol >= 0 )
	  sdata.push( sval );
      }
    } while ( sf.readDataLine( stopempty ) );

    if ( xdata.size() >= minn ) {
      if ( xcol.size() > 1 )
	xdata -= x2data;
      if ( ycol < 0 )
	analyseData( xdata, sdata, page, statskey );
      else
	analyseCor( xdata, ydata, sdata, page, statskey );
    }

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
  cerr << "datastats -d ### -D -c ### [-y ###] [-s ###] -e ### -E ### -z -m ###\n";
  cerr << "          -a aaa -q -f ### -t ### -k -K -n -o xxx fname\n";
  cerr << '\n';
  cerr << "basic statistics of one column in data file <fname>.\n";
  cerr << "-c: ### specifies column (default is first column).\n";
  cerr << "-x  the column can be either a number (1,2,...)\n";
  cerr << "    or a string containing the column title.\n";
  cerr << "    If two x-columns are specified, then the difference x1-x2 is analysed.\n";
  cerr << "-y: ### specifies a second data column (default is no column).\n";
  cerr << "-s: ### specifies a column that holds the standard deviations for the\n";
  cerr << "    x or y-column (default is no column).\n";
  cerr << "-e: exclude x-column values less than or equal to ###.\n";
  cerr << "-E: exclude x-column values greater than or equal to ###.\n";
  cerr << "-z: discard s-column values less than or equal to zero.\n";
  cerr << "-m: ### The minimum number of data lines required for analysis\n";
  cerr << "    (default 1 for single variable, 2 for two variables).\n";
  cerr << "-a: value that is added to the table (either column number, column title\n";
  cerr << "    (takes the value from the last line), 'ident:value', or 'l_ident'\n";
  cerr << "    (take value of ident in meta data level l, l can be ommited))\n";
  cerr << "-q: don't write statistics to standard error\n";
  cerr << "-t: ### threshold value for count statistics (default 0.0)\n";
  cerr << "-f: select output columns...\n";
  cerr << "  ...for one variable:\n";
  cerr << "    a: average\n";
  cerr << "    s: standard deviation\n";
  cerr << "    e: standard error mean\n";
  cerr << "    v: variance\n";
  cerr << "    c: coefficient of variation\n";
  cerr << "    z: sum of the data values\n";
  cerr << "    m: median\n";
  cerr << "    q: 1. and 3. quartile\n";
  cerr << "    d: 1. and 9. decile\n";
  cerr << "    x: minimum and maximum\n";
  cerr << "    w: maximum minus minimum\n";
  cerr << "    < or -: number of data values smaller than threshold\n";
  cerr << "    > or +: number of data values greater than threshold\n";
  cerr << "    n: number of data points\n";
  cerr << "    S: Sign test for difference of median to threshold value and significance\n";
  cerr << "    t: Student's t for difference of mean to threshold value and significance\n";
  cerr << "    n: number of data points\n";
  cerr << "  ...for two variables:\n";
  cerr << "    m: slope with standard deviation\n";
  cerr << "    b: offset with standard deviation\n";
  cerr << "    r: correlation coefficient and significance of r\n";
  cerr << "    z: Fisher's z for the correlation coefficient r\n";
  cerr << "    u: unpaired t-Test (Student's t and significance)\n";
  cerr << "    p: paired t-Test (Student's t and significance)\n";
  cerr << "    U: Mann-Whitney U-Test for unpaired comparisons of medians\n";
  cerr << "       (U and significance)\n";
  cerr << "    W: Wilcoxon-test for paired comparisons of medians (W and significance)\n";
  cerr << "    F: F-test for significantly different variances (F and significance)\n";
  cerr << "    k: Kendall's tau for nonparametric correlation (tau and significance)\n";
  cerr << "    d: mean difference y-x with standard-deviation\n";
  cerr << "    n: number of data points\n";
  cerr << "-k: add key to the output table\n";
  cerr << "-K: just print the key, don't process data\n";
  cerr << "-n: number columns of the key\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-D: more than one space between data columns required.\n";
  cerr << "-o: write results into file ### instead to standard out\n";
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
  bool alabel = false;
  while ( (c = getopt( argc, argv, "d:c:x:y:s:e:E:zm:a:o:f:t:kKDqn" )) >= 0 ) {
    switch ( c ) {
      case 'x':
      case 'c': if ( optarg != NULL ) {
                  int xc = 0;
	          if ( sscanf( optarg, "%d", &xc ) == 0 ) {
		    xcol.push_back( 0 );
		    xcols.push_back( optarg );
		  }
                  else {
		    xcol.push_back( xc-1 );
		    xcols.push_back( "" );
		  }
                }
                break;
      case 'y': if ( optarg != NULL ) {
	          if ( sscanf( optarg, "%d", &ycol ) == 0 )
		    ycols = optarg;
                  else 
                    ycol--;
                }
                break;
      case 's': if ( optarg != NULL ) {
	          if ( sscanf( optarg, "%d", &scol ) == 0 )
		    scols = optarg;
                  else 
                    scol--;
                }
                break;
      case 'e': if ( optarg != NULL ) {
		  double v = 0.0;
	          if ( sscanf( optarg, "%lf", &v ) > 0 )
		    xmin = v;
                }
                break;
      case 'E': if ( optarg != NULL ) {
		  double v = 0.0;
	          if ( sscanf( optarg, "%lf", &v ) > 0 )
		    xmax = v;
                }
                break;
      case 'z': ignorezero = true;
                break;
      case 'm': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &minn ) == 0 ||
		     minn < 1 )
		  minn = 1;
                break;
      case 'a': if ( optarg != NULL ) {
                  Str as = optarg;
		  string aident = as.ident();
		  Str val = as.value();
		  if ( ! alabel ) {
		    statskey.addLabel( "parameter" );
		    alabel = true;
		  }
		  if ( ! aident.empty() && ! val.empty() ) {
		    double e;
		    string aunit = "";
		    double aval = val.number( e, aunit );
		    statskey.addNumber( aident, aunit, "%7.5g" );
		    statskey.setNumber( aident, aval );
		  }
		  else {
		    acols.push_back( as );
		    acol.push_back( statskey.columns() );
		    statskey.addNumber( as, "-", "%7.5g" );
		  }
		}
                break;
      case 'f': if ( optarg != NULL )
	          outformat = optarg;
    	        break;
      case 't': if ( optarg == NULL ||
		     sscanf( optarg, "%lg", &threshold ) == 0 )
		  threshold = 0.0;
                break;
      case 'k': key = true;
                break;
      case 'K': key = true;
	        keyonly = true;
                break;
      case 'd': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &stopempty ) == 0 ||
		     stopempty < 1 )
		  stopempty = 1;
                break;
      case 'D': dblankmode = true;
                break;
      case 'o': if ( optarg != NULL )
		  statsfile = optarg;
                break;
      case 'n': numbercols = true;
                break;
      case 'q': datamode = false;
                break;
      default : WriteUsage();
    }
  }
  if ( optind < argc && argv[optind][0] == '?' ) {
    WriteUsage();
  }
  filec = optind;
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
    else {
      sf.open( cin );
    }
  }

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( !statsfile.empty() ) {
    df.open( statsfile.c_str() );
    if ( !df.good() ) {
      cerr << "! can't open file " << statsfile << " for writing\n";
      return 1;
    }
    coutb = cout.rdbuf();
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }
  else {
    datamode = false;
  }

  readData( sf );

  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
