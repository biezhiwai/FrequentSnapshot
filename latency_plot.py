__author__ = 'mk'
import matplotlib.pyplot as plt
import sys
unitSize = sys.argv[1];
unitNum = sys.argv[2];
uf = sys.argv[3];
threadID = sys.argv[4];
dataDir = sys.argv[5];
resultDir = sys.argv[6];
algLabel = ['naive', 'cou', 'zigzag', 'pingpong', 'MK', 'LL']
fig = plt.figure(figsize=(8, 4))

def init():
    plt.xlabel("Tick Count")
    plt.ylabel("Latency Distribution[sec]")
    plt.title("Latency Evoluation")

def gen(xmin, xmax, ymin, ymax):
    plt.xlim(xmin, xmax)
    plt.ylim(ymin, ymax)
	#fig.yscale('log')
    fig.canvas.draw()


def loadlog():
    for i in range(0 , 6, 1):
        logPath = dataDir + str(i) + "_latency_" + str(uf) + "k_" + str(unitNum) + "_" + str(unitSize) + "_" + str(threadID) + ".log"
        logFile = open(logPath)
        tick = []
        latency = []
        count = 0
        for eachLine in logFile.readlines():
            timeMsStr, latencyMsStr = eachLine.split(",")
            latencyMs = float(latencyMsStr)
            count = count + 1
            tick.append(count)
			#us normalization
            latency.append(latencyMs/1000000.0)

        plt.plot(tick, latency, label=algLabel[i], linewidth=1)
        logFile.close()
    plt.legend()

init()
loadlog()
#plt.xlim( 1000 , 2500)
#plt.ylim(0 , 0.1)
plt.yscale('log')
plt.savefig(resultDir + "Latency" + str(uf) + "k.pdf")
#fig.show()
