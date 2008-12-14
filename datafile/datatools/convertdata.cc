/*
  convertdata.cc
  

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
#include <getopt.h>
#include <vector>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>

using namespace std;
using namespace relacs;

int stopempty = 2;
bool numbercols = false;
bool units = true;
bool bodyonly = false;
int sectionlevel = -1;
string imagetag = "";
string format = "l";
string destfile = "";


void writeLaTeX( DataFile &sf )
{
  static const string latexsections[5] = 
    { "\\section", "\\subsection", "\\subsubsection",
      "\\paragraph", "\\subparagraph" };

  if ( ! bodyonly ) {
    cout << "\\documentclass{article}\n";
    cout << "\n";
    cout << "\\usepackage[margin=15mm,noheadfoot]{geometry}\n";
    cout << "\\usepackage{graphics}\n";
    cout << "\n";
    cout << "\\begin{document}\n";
    cout << "\n";
  }
  while ( sf.good() ) {
    sf.readMetaData();

    cout << "\\begin{minipage}{\\textwidth}\n";

    // write out new meta data:
    for ( int l=sf.levels()-1; l>=0; l-- ) {
      if ( sf.newMetaData( l ) ) {
	bool namevals = false;
	bool para = false;
	for ( int k=0; k<sf.metaData( l ).size(); k++ ) {
	  Str ml = sf.metaData( l )[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  string ident = ml.ident().latex();
	  string value = ml.value().latexUnit();
	  if ( ident.empty() || value.empty() ) {
	    if ( namevals ) {
	      cout << "\\end{tabular}\n";
	      namevals = false;
	    }
	    if ( sectionlevel >= 0 && k == 0 )
	      cout << latexsections[sectionlevel+sf.levels()-l-1] << "{"
		   << ml.latex() << "}\n";
	    else {
	      if ( ! para ) {
		cout << '\n';
		para = true;
	      }
	      cout << ml.latex() << '\n';
	    }
	  }
	  else {
	    if ( para ) {
	      cout << '\n';
	      para = false;
	    }
	    if ( ! imagetag.empty() && ident == imagetag ) {
	      if ( namevals ) {
		cout << "\\end{tabular}\n";
		namevals = false;
	      }
	      cout << "\\includegraphics{" << value << "}\n";
	    }
	    else {
	      if ( ! namevals ) {
		cout << "\\begin{tabular}{ll}\n";
		namevals = true;
	      }
	      cout << "  " << ident << ": & " << value << " \\\\\n";
	    }
	  }
	}
	if ( namevals )
	  cout << "\\end{tabular}\n";
	cout << '\n';
      }
    }

    // write out key:
    sf.key().saveKeyLaTeX( cout, numbercols, units );

    // write out data:
    int dcs = sf.dataComments().size();
    do {
      int ndcs = sf.dataComments().size();
      if ( ndcs > dcs ) {
	for ( int k=dcs; k<ndcs; k++ ) {
	  Str ml = sf.dataComments()[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  cout << "  \\multicolumn{2}{l}{" << ml.latex() << "}\\\\\n";
	}
	dcs = ndcs;
      }
      StrQueue items;
      sf.splitLine( items );
      StrQueue latexitems;
      for ( int k=0; k<items.size(); k++ ) {
	if ( Str::FirstNumber.find( items[k][0] ) >= 0 )
	  latexitems.add( items[k].latexNum() );
	else
	  latexitems.add( "\\multicolumn{1}{l}{" + items[k].latex() + "}" );
      }
      cout << "  " << latexitems.save( " & " ) << " \\\\\n";
    } while ( sf.readDataLine( stopempty ) );
    cout << "  \\hline\n";
    cout << "\\end{tabular}\n";
    cout << "\\end{minipage}\n";
    cout << "\\vspace{2ex}\n";
    cout << "\n";

  }
  if ( ! bodyonly ) {
    cout << "\n";
    cout << "\\end{document}\n";
  }
  sf.close();
}


void writeHTML( DataFile &sf )
{
  if ( ! bodyonly ) {
    cout << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n";
    cout << "<html>\n";
    cout << "  <head>\n";
    cout << "    <title>Data</title>\n";
    cout << "    <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\" >\n";
    cout << "    <style type=\"text/css\">\n";
    cout << "    <!--\n";
    cout << "    .metaimage {\n";
    cout << "      padding-top: 5px;\n";
    cout << "      padding-bottom: 5px;\n";
    cout << "    }\n";
    cout << "    table.data {\n";
    cout << "      border-top: 1px solid black;\n";
    cout << "      border-bottom: 1px solid black;\n";
    cout << "    }\n";
    cout << "    table.data thead tr.dataunits {\n";
    cout << "      border-bottom: 1px solid black;\n";
    cout << "    }\n";
    cout << "    table.data th {\n";
    cout << "      padding-left: 0.5em;\n";
    cout << "      padding-right: 0.5em;\n";
    cout << "      white-space: nowrap;\n";
    cout << "    }\n";
    cout << "    table.data td {\n";
    cout << "      padding-left: 0.5em;\n";
    cout << "      padding-right: 0.5em;\n";
    cout << "      white-space: nowrap;\n";
    cout << "    }\n";
    cout << "    -->\n";
    cout << "    </style>\n";
    cout << "  </head>\n";
    cout << "\n";
    cout << "  <body>\n";
    cout << "\n";
  }
  while ( sf.good() ) {
    sf.readMetaData();

    cout << "    <div class=\"datablock\">\n";

    // write out new meta data:
    for ( int l=sf.levels()-1; l>=0; l-- ) {
      if ( sf.newMetaData( l ) ) {
	cout << "      <div class=\"metalevel" << l+1 << "\">\n";
	bool namevals = false;
	bool para = false;
	for ( int k=0; k<sf.metaData( l ).size(); k++ ) {
	  Str ml = sf.metaData( l )[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  string ident = ml.ident().html();
	  string value = ml.value().htmlUnit();
	  if ( ident.empty() || value.empty() ) {
	    if ( namevals ) {
	      cout << "        </table>\n\n";
	      namevals = false;
	    }
	    if ( sectionlevel >= 0 && k == 0 ) {
	      int hlevel = sectionlevel+sf.levels()-l;
	      cout << "        <h" << hlevel << ">"
		   << ml.html() << "</h" << hlevel << ">\n";
	    }
	    else {
	      if ( ! para ) {
		cout << "        <p>\n";
		para = true;
	      }
	      cout << "          " << ml.html() << '\n';
	    }
	  }
	  else {
	    if ( para ) {
	      cout << "        </p>\n";
	      para = false;
	    }
	    if ( ! imagetag.empty() && ident == imagetag ) {
	      if ( namevals ) {
		cout << "        </table>\n";
		namevals = false;
	      }
	      cout << "        <div class=\"metaimage\">\n";
	      cout << "          <img src=\"" << value << ".png\" alt=\"" << value << "\">\n";
	      cout << "        </div>\n";
	    }
	    else {
	      if ( ! namevals ) {
		cout << "        <table class=\"metadata\">\n";
		namevals = true;
	      }
	      cout << "          <tr>\n";
	      cout << "            <td>" << ident << ":</td>\n";
	      cout << "            <td>" << value << "</td>\n";
	      cout << "          </tr>\n";
	    }
	  }
	}
	if ( namevals )
	  cout << "        </table>\n";
	if ( para )
	  cout << "        </p>\n";
	cout << "      </div>\n";
      }
    }

    // write out key:
    sf.key().saveKeyHTML( cout, numbercols, units );

    // write out data:
    cout << "        <tbody class=\"data\">\n";
    int dcs = sf.dataComments().size();
    do {
      int ndcs = sf.dataComments().size();
      if ( ndcs > dcs ) {
	for ( int k=dcs; k<ndcs; k++ ) {
	  Str ml = sf.dataComments()[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  cout << "          <tr class=\"datacomment\">\n";
	  cout << "            <td colspan=\"2\" align=\"left\">" << ml.html() << "</td>\n";
	  cout << "          </tr>\n";
	}
	dcs = ndcs;
      }
      StrQueue items;
      sf.splitLine( items );
      cout << "          <tr class=\"data\">\n";
      for ( int k=0; k<items.size(); k++ ) {
	if ( Str::FirstNumber.find( items[k][0] ) >= 0 )
	  cout << "            <td align=\"right\">" << items[k].htmlUnit() << "</td>\n";
	else
	  cout << "            <td align=\"left\">" << items[k].html() << "</td>\n";
      }
      cout << "          </tr>\n";
    } while ( sf.readDataLine( stopempty ) );
    cout << "        </tbody>\n";
    cout << "      </table>\n";
    cout << "    </div>\n";
    cout << "\n";

  }
  if ( ! bodyonly ) {
    cout << "\n";
    cout << "  </body>\n";
    cout << "</html>\n";
  }
  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "convertdata [-d ###] [-n] [-U] [-s[#]] [-i[#]] [-f #] [-b] [-o xxx] <fname>\n";
  cerr << '\n';
  cerr << "Convert the data file <fname> into a different format.\n";
  cerr << "-f: format of the converted data:\n";
  cerr << "    l - LaTeX\n";
  cerr << "    h - HTML\n";
  cerr << "-b: (body only) omit any headers and footers\n";
  cerr << "-n: number columns of the key\n";
  cerr << "-U: don't print the line with the units in the key\n";
  cerr << "-s: make the first line of each meta-data block that is not\n";
  cerr << "    a name-value pair a section title\n";
  cerr << "    The section level can be increased by # (default 0), e.g. -s1.\n";
  cerr << "-i: interpret #  in meta data as image files (default \"image\")\n";
  cerr << "-d: the number of empty lines that separate blocks of data (default: 2).\n";
  cerr << "-o: write converted data into file ### instead to standard out\n";
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
  while ( (c = getopt( argc, argv, "d:o:f:s::i::nUb" )) >= 0 ) {
    switch ( c ) {
    case 'd':
      if ( optarg == NULL ||
	   sscanf( optarg, "%d", &stopempty ) == 0 ||
	   stopempty < 1 )
	stopempty = 1;
      break;
    case 'o':
      if ( optarg != NULL )
	destfile = optarg;
      break;
    case 'n':
      numbercols = true;
      break;
    case 'U':
      units = false;
      break;
    case 's':
      if ( optarg == NULL ||
	   sscanf( optarg, "%d", &sectionlevel ) == 0 )
	sectionlevel = 0;
      break;
    case 'i':
      if ( optarg == NULL )
	imagetag = "image";
      else
	imagetag = optarg;
      break;
    case 'b':
      bodyonly = true;
      break;
    case 'f':
      format = optarg;
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
  int filec = 0;
  readArgs( argc, argv, filec );

  // redirect cin:
  DataFile sf;
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

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( !destfile.empty() ) {
    df.open( destfile.c_str() );
    if ( !df.good() ) {
      cerr << "! can't open file " << destfile << " for writing\n";
      return 1;
    }
    coutb = cout.rdbuf();
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }

  if ( format == "l" )
    writeLaTeX( sf );
  else if ( format == "h" )
    writeHTML( sf );
  else {
    cerr << "! unknown format !\n";
    WriteUsage();
  }

  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
