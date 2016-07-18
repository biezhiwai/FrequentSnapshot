__author__ = 'mk'

import matplotlib.pyplot as plt
import sys
import math
import numpy as np
dataDir = sys.argv[1]
resDir = sys.argv[2]

fig = plt.figure(figsize=(8,4))
algLabel=['naive','cou','zigzag','pingpong','MK','LL']
marker_set=['^','X','.','*','+','o']
for i in range(0,6,1):
    size = 1
    filePath = dataDir + str(i) + '_overhead.dat'
    file = open(filePath)
    x = []
    y = []
    for eachLine in file.readlines():
        xStr,yStr = eachLine.split()
        x.append(size)
        size += 1
        y.append(float(yStr)/1000000.0)
    file.close()
    plt.plot(x,y,label=algLabel[i],linewidth=0.1,marker=marker_set[i],markersize=3)

plt.xlim(0.8, 5.2)
plt.xticks([1,2,3,4,5],['100','200','400','800','1600'])
plt.yscale('log')
plt.xlabel('Database Size[MB],logscale')
plt.ylabel('Overhead per Checkpoint[sec],logscale')
#plt.title('Overhead Per Checkpoint')
plt.legend(loc='upper left')

plt.savefig(resDir + "OverheadPerCheckpoint.pdf")

