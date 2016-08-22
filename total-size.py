import numpy as np
import sys

preparedir = "./log/overhead/"
for i in range(0,6,1):
    for size in [250000,500000,1000000,2000000]:
        prepare_path=preparedir + str(i) + "_overhead_256k_" + str(size) + "_4096.log"
        prepare_file = open(prepare_path)
        totals = []
        for el in prepare_file.readlines():
            el.replace(' ','')
            prepare,overhead,total = el.split(",")
            totals.append(float(total))
        var=np.mean(totals)
        print(var)





