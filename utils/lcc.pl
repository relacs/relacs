#!/usr/bin/perl

# lcc.pl
# converts filenames given in the argument to lowercase.
# renames *.hh files to *.h
# does the same with all #include files.

###############################################################################
#
# lcc.pl
#
#
# RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
# Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# RELACS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################

while ( $sourcefile = shift @ARGV ) {

  $destfile = lc( $sourcefile );
  $destfile =~ s/.hh/.h/;

  open( SF, "<$sourcefile" ) or die "can't open file\n";
  open( DF, ">$destfile" ) or die "can't open file\n";
  while ( $line = <SF> ) {
    if ( $line =~ /#include/ ) {
      $line = lc( $line );
      $line =~ s/.hh/.h/;
    }
    print DF $line;
  }
  close DF;
  close SF;

  `rm $sourcefile`;
}

