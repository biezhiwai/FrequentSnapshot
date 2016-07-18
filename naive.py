__author__ = 'mk'
import matplotlib.pyplot as plt

unitSize = 8192
unitNum = 25600
uf = 1
threadID = 0
dataDir = "./log/latency/"
resultDir = "./diagrams/experimental_result/"
algLabel = ['naive', 'cou', 'zigzag', 'pingpong', 'MK', 'LL']
fig = plt.figure(figsize=(8, 4))

def init():
    plt.xlabel("time(ns)")
    plt.ylabel("Latency")
    plt.title("Latency Test")

def gen(xmin, xmax, ymin, ymax):
    plt.xlim(xmin, xmax)
    plt.ylim(ymin, ymax)
	#fig.yscale('log')
    fig.canvas.draw()

# plt.savefig(resultDir + "Latency" + str(uf) + "k.pdf")

def loadlog():
    for i in range(0 , 1, 1):
        logPath = dataDir + str(i) + "_latency_" + str(uf) + "k_" + str(unitNum) + "_" + str(unitSize) + "_" + str(threadID) + ".log"
        logFile = open(logPath)
        tick = []
        latency = []
        count = 0
        for eachLine in logFile.readlines():
            timeNsStr, latencyNsStr = eachLine.split(",")
            latencyNs = float(latencyNsStr)
            count = count + 1
            tick.append(count)
            #latency.append(latencyNs / 1000000000.0)
			#ms
            latency.append(latencyNs)
            #latency.append(latencyNs / 1000)

        plt.plot(tick, latency, label=algLabel[i], linewidth=1)
        logFile.close()
    plt.legend()

init()
loadlog()
fig.show()
