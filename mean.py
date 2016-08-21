import numpy as np
import sys

latencydir = "./log/latency/"
preparedir = "./log/overhead/"
for i in range(0,6,1):
    for uf in [16,32,64,128,256]:
        latency_path=latencydir + str(i) + "_latency_" + str(uf) + "k_250000_4096_0.log"
        prepare_path=preparedir + str(i) + "_overhead_" + str(uf) + "k_250000_4096.log"
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





