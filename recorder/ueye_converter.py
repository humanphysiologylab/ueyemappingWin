import scipy.io as sio
import numpy as np

width = 128
height = 128

load = np.fromfile("sequence4.bin", dtype = 'int16', sep = "")
print("loaded")

length = 460

cmosDataNp = np.zeros((width,height,length))

for t in range(length):
    if t % 100 == 0:
        print(t, " ms done")
    for i in range(width):
        for j in range(height):
            cmosDataNp[i,j,t] = load[j+i*128+t*128*128]
print("reshaped")

mat_file = {}

#__header__

mat_file['__header__'] = "MATLAB 5.0 MAT-file, Platform: PCWIN64, Created on: Thu Jan 01 00:00:00 1970"

#__version__

mat_file['__version__'] = "1.0"

#__globals__

mat_file['__globals__'] = []

#cmosData

mat_file['cmosData'] = cmosDataNp

#acqFreq

mat_file['acqFreq'] = 2

#dual

mat_file['dual'] = 4

#frequency

mat_file['frequency'] = 230

#cmosData2

mat_file['cmosData2'] = []

#channel

#mat_file['channel'] = [0, 0, 0, ..., 0, 0, 0]
                                    
#bgimage2

mat_file['bgimage2'] = []

#bgimage

mat_file['bgimage'] = cmosDataNp[:,:,200]



sio.savemat('sequence.mat', mat_file)
