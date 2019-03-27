import numpy as np
import sys

latencydir = "./log/latency/"
preparedir = "./log/overhead/"
for i in range(0,6,1):
    for size in [250000,500000,1000000,2000000]:
        latency_path=latencydir + str(i) + "_latency_256k_" + str(size) + "_4096_0.log"
        prepare_path=preparedir + str(i) + "_overhead_256k_" + str(size) + "_4096.log"
        latency_file = open(latency_path)
        prepare_file = open(prepare_path)
        latencys = []
        prepares = []
        for eachline in latency_file.readlines():
            timestart, time = eachline.split(",")
            latencys.append(float(time))
        for el in prepare_file.readlines():
            el.replace(' ','')
            prepare,overhead,total = el.split(",")
            prepares.append(float(prepare))
        var=(np.sum(latencys)-np.sum(prepares))/(len(latencys)-len(prepares))
        print( var)





