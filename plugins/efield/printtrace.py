# example script for printing all visible traces
# set plotcommand to "python printtrace.py" in relacs.cfg

import numpy as np
import matplotlib.pyplot as plt
import subprocess


def is_float( s ):
    try:
        float(s)
        return True
    except ValueError:
        return False
   
def loaddat( filename ):
    """ Load ascii data files into a numpy array
    """
    header = {}
    key = []
    data = []
    inkey = False
    for l in open( filename ) :
        if l.startswith( "#" ) :
            if l.startswith( "#Key" ) :
                inkey = True
            elif ":" in l :
                inkey = False
                tmp = [e.strip() for e in l[1:].partition(':')]
                header[tmp[0]] = tmp[2]
            elif inkey :
                key.append( l.split()[1:] )
        elif l and not l.isspace() :
            inkey = False
            data.append( [ float( i ) for i in l.split() if is_float( i ) ] )
    return np.array( data ), key, header


# load data:
data, key, header = loaddat( "traces.dat" )

# plot data:
ncols = len( key[-1] )-1
fw = 18.0
fh = 24.0
if ncols == 1 :
    fh = 12.0
fig = plt.figure( figsize=( fw/2.54, fh/2.54 ) )
for col in xrange( 1, ncols+1 ) :
    ax = fig.add_subplot( ncols, 1, col )
    if col == 1 :
        ax.set_title( header['Species'] + ", EODf=" + header['EOD Rate'], fontsize=18 )
    ax.set_xlim( data[0,0], data[-1,0] )
    ax.set_xlabel( key[0][0] + ' [' + key[1][0] + ']' )
    ax.set_ylabel( key[0][col] + ' [' + key[1][col] + ']' )
    ax.ticklabel_format(useOffset=False)
    ax.plot( data[:,0], data[:,col] )
plt.tight_layout()
plt.savefig( 'traces.pdf', papertype="a4" )

# fix pdf file and print:
subprocess.call( 'pdfjam -q -o /dev/stdout traces.pdf 1 | lpr', shell=True )
