#!/usr/bin/env python
import numpy as np

preparedir = "./log/overhead/"
for i in [0, 1, 2, 3, 4, 5, 6]:
    for uf in [16, 32, 64, 128, 256]:
        prepare_path = preparedir + str(i) + "_overhead_" + str(uf) + "k_250000_4096.log"
        prepare_file = open(prepare_path)
        line_cnt = 0
        prepares = []
        totals = []
        for el in prepare_file.readlines():
            prepare, overhead, total = el.split("\t")
            if line_cnt > 0:
                prepares.append(float(prepare))
                totals.append(float(total))
            line_cnt = line_cnt + 1
        str2 = "%d\t%d" % (np.mean(prepares), np.mean(totals))
        print(str2)
    for size in [500000, 1000000, 2000000]:
        prepare_path = preparedir + str(i) + "_overhead_256k_" + str(size) + "_4096.log"
        prepare_file = open(prepare_path)
        line_cnt = 0
        prepares = []
        totals = []
        for el in prepare_file.readlines():
            prepare, overhead, total = el.split("\t")
            if line_cnt > 0:
                prepares.append(float(prepare))
                totals.append(float(total))
            line_cnt = line_cnt + 1
        str2 = "%d\t%d" % (np.mean(prepares), np.mean(totals))
        print(str2)
