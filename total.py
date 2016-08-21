import numpy as np
import sys

preparedir = "./log/overhead/"
for i in range(0,6,1):
    for uf in [16,32,64,128,256]:
        prepare_path=preparedir + str(i) + "_overhead_" + str(uf) + "k_250000_4096.log"
        prepare_file = open(prepare_path)
        totals = []
        for el in prepare_file.readlines():
            el.replace(' ','')
            prepare,overhead,total = el.split(",")
            totals.append(float(total))
        var=np.mean(totals)
        print(var)





