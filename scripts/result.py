# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import sys
import os

uf = sys.argv[1]
size = sys.argv[2]
page = sys.argv[3]
hpr = sys.argv[4]
alpha = sys.argv[5]
path = sys.argv[6]
os.makedirs(path, exist_ok=True)
f1 = open("./" + path + "/avg_tick_latency.csv", "a")
f2 = open("./" + path + "/avg_prepare.csv", "a")
f3 = open("./" + path + "/avg_ckp_overhead.csv", "a")
f5 = open("./" + path + "/max_tick_latency.csv", "a")
head = uf + "k_" + size + "_" + page + "_" + alpha
f1.write(head)
f2.write(head)
f3.write(head)
f5.write(head)

# for i in [0, 1, 2, 3, 4, 8]:
for i in [2,8]:
    latency_path = "./log/latency_" + str(i) + "_" + uf + "k_" + size + "_" + page + "_0" + "_" + alpha + "_" + hpr + ".log"
    prepare_path = "./log/" + str(i) + "_" + uf + "k_" + size + "_" + page + "_" + alpha + "_" + hpr + "_overhead.log"

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
    f1.write(", " + "{:.2f}".format(mean_tick_latency))
    f2.write(", " + "{:.2f}".format(mean_prepare))
    f3.write(", " + "{:.2f}".format(mean_ckp_overheads))
    f5.write(", " + "{:.2f}".format(max_tick_latency))
f1.write("\n")
f2.write("\n")
f3.write("\n")
f5.write("\n")
f1.close()
f2.close()
f3.close()
f5.close()