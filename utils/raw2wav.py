import numpy as np
from scipy.io.wavfile import write
import matplotlib.pyplot as plt

filename = 'trace-1.raw'
rate = 40000

x = np.fromfile( filename, np.float32)
x -= np.mean( x )

#plt.hist( x, 100 )
#plt.show()

min = -np.min( x )
max = np.max( x )
max = np.max( ( min, max ) )

x /= max

d = np.array( np.round( x * 2**15 ), dtype='<i2' )
write( 'test.wav', rate, d )

