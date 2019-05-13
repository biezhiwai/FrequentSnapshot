# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import sys

i = sys.argv[1]
uf = sys.argv[2]
size = sys.argv[3]
page = sys.argv[4]

latency_path = "./log/latency_" + str(i) + "_" + str(uf) + "k_" + str(size) + "_" + str(page) + "_0.log"
prepare_path = "./log/" + str(i) + "_" + str(uf) + "k_" + str(size) + "_" + str(page) + "_overhead.log"

latency_file = open(latency_path)
prepare_file = open(prepare_path)
latencys = []
prepares = []
overheads = []
for eachline in latency_file.readlines():
    latencys.append(float(eachline))
for el in prepare_file.readlines():
    el.replace(' ', '')
    prepare, overhead, total = el.split("\t")
    prepares.append(float(prepare))
    overheads.append(float(overhead))

mean1 = (np.sum(latencys) - np.sum(prepares) / 1000.0) / (len(latencys) - len(prepares))
mean2 = np.mean(prepares)
mean3 = np.mean(overheads)
str2 = "%d\t%d\t%d" % (mean1, mean2, mean3)
print(str2)
