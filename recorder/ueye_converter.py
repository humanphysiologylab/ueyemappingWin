import scipy.io as sio
import numpy as np

meta = open("output.txt", "r")
fps = meta.read()
meta.close()

meta = open("config.txt", "r")
time = int(meta.readline())

load = np.fromfile("sequence.bin", dtype = 'int16', sep = "")
print("loaded")

width = 128
height = 128

length = int(load.size/(width*height))

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

mat_file['dual'] = 0

#frequency

mat_file['frequency'] = fps

#channel

#mat_file['channel'] = [0, 0, 0, ..., 0, 0, 0]

#bgimage

mat_file['bgimage'] = cmosDataNp[:,:,200]



sio.savemat('sequence.mat', mat_file)
