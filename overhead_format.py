__author__ = 'mk'
import sys

def get_ckp_overhead(algType, uf, dbSize, unitSize, dataDir):
    logPath = dataDir + str(algType) + "_overhead_" + str(uf) + "k_" + str(dbSize) + "_" + str(unitSize) + ".log"
    file = open(logPath)
    file.readline()
    file.readline()
    overheadList = []
    for eachLine in file.readlines():
        prepare, overhead, total = eachLine.split()
        overheadList.append(int(total))

    avg = sum(overheadList) / len(overheadList)
    file.close()
    return avg

arg = sys.argv[1]
algType, Size, baseUF, unitSize, resultDir, dataDir = arg.split()

resultFile = open(str(resultDir) + str(algType) + "_overhead.dat", "a")
line = str(Size) + "\t" + str(get_ckp_overhead(algType, baseUF, Size, unitSize, dataDir)) + "\n"
resultFile.write(line)
resultFile.close()
