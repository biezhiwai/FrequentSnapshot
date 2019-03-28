#!/usr/bin/env python
import numpy as np
import sys

latencydir = "../log/latency/"
preparedir = "../log/overhead/"
for i in [0,1,2,3,4,5,6]:
    for uf in [16,32,64,128,256]:
        latency_path=latencydir + str(i) + "_latency_" + str(uf) + "k_250000_4096_0.log"
        prepare_path=preparedir + str(i) + "_overhead_" + str(uf) + "k_250000_4096.log"
        latency_file = open(latency_path)
        prepare_file = open(prepare_path)
        latencys = []
        prepares = []
        for eachline in latency_file.readlines():
            timestart, time = eachline.split("\t")
            latencys.append(float(time))
        for el in prepare_file.readlines():
            el.replace(' ','')
            prepare,overhead,total = el.split("\t")
            prepares.append(float(prepare))
        mean=(np.sum(latencys)-np.sum(prepares))/(len(latencys)-len(prepares))
        str2 = "%d" %(mean)
        print(str2)
    for size in [500000,1000000,2000000]:
		latency_path=latencydir + str(i) + "_latency_256k_" + str(size) + "_4096_0.log"
		prepare_path=preparedir + str(i) + "_overhead_256k_" + str(size) + "_4096.log"
		latency_file = open(latency_path)
		prepare_file = open(prepare_path)
		latencys = []
		prepares = []
		for eachline in latency_file.readlines():
			timestart, time = eachline.split("\t")
			latencys.append(float(time))
		for el in prepare_file.readlines():
			el.replace(' ','')
			prepare,overhead,total = el.split("\t")
			prepares.append(float(prepare))
		mean=(np.sum(latencys)-np.sum(prepares))/(len(latencys)-len(prepares))
		str2 = "%d" %(mean)
		print(str2)
