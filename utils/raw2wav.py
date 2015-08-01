import sys
import os
import numpy as np
import scipy.io.wavfile as wav
import matplotlib.pyplot as plt

if sys.argv > 1 :
    filepathes = sys.argv[1:]
else :
    filepath = [ 'trace-1.raw' ]
rate = 40000

data = None
for path in filepathes :
    print 'path=', path
    x = np.fromfile( path, np.float32)
    x -= np.mean( x )

    min = -np.min( x )
    max = np.max( x )
    max = np.max( ( min, max ) )

    x /= max

    d = np.array( np.round( x * 2**15 ), dtype='<i2' )
    if data == None :
        data = d
    else :
        data = np.vstack( ( data, d ) )

filename = os.path.basename( filepathes[0] )
name = filename.split( '.' )[0]
ext = filename.split( '.' )[-1]
wavfile = name + '.wav'
wav.write( wavfile, rate, data.T )
print 'converted', filepathes[0], 'to', wavfile

