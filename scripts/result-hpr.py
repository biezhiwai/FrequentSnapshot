# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import sys
import os

i = str(8)
uf = sys.argv[1]
size = sys.argv[2]
page = sys.argv[3]
hpr = sys.argv[4]
alpha = sys.argv[5]
path = sys.argv[6]
os.makedirs(path, exist_ok=True)
f1 = open("./" + path + "/result-hpr.csv", "a")

head = i + "_" + uf + "k_" + size + "_" + page + "_" + alpha + "_" + hpr
f1.write(head)



latency_path = "./log/latency_" + i + "_" + uf + "k_" + size + "_" + page + "_0" + "_" + alpha + "_" + hpr + ".log"
prepare_path = "./log/" + i + "_" + uf + "k_" + size + "_" + page + "_" + alpha + "_" + hpr + "_overhead.log"

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

mean_tick_latency = (np.sum(latencys) - np.sum(prepares) / 1000.0) / (len(latencys) - len(prepares))
max_tick_latency = np.max(latencys)
mean_prepare = np.mean(prepares)
mean_ckp_overheads = np.mean(overheads)

f1.write("," + "{:.2f}".format(mean_tick_latency))
f1.write("," + "{:.2f}".format(mean_prepare))
f1.write("," + "{:.2f}".format(mean_ckp_overheads))
f1.write("," + "{:.2f}".format(max_tick_latency))
f1.write("\n")
f1.close()
