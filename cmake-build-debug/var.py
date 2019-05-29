import numpy as np
import sys

path = sys.argv[1];
file = open(path);
latency = []
for eachline in file.readlines():
    timestart, time = eachline.split(",")
    latency.append(float(time))
# arr=np.array(latency)
# sum1=arr.sum()
# latency2=arr*arr
# sum2=latency2.sum()
# mean=sum1/len(arr)
# var=sum2/len(arr)-mean**2

var = np.var(latency)
print(var)
