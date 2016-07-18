__author__ = 'mk'

import numpy
import sys

fileName = sys.argv[1]
zipfSize = int(sys.argv[2]) * 1000
zipfMax = int(sys.argv[3])

s = numpy.random.zipf(2,zipfSize)

zipfFile = open(fileName,"w")

for eachItem in s:
    if eachItem > zipfMax:
        eachItem = eachItem % zipfMax
    zipfFile.write(str(eachItem) + '\n')
zipfFile.close()
