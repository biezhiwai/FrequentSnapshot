import matplotlib.pyplot as plt
import numpy as np



linestyles={'alpha=1.1':'-','alpha=1.3':'--','alpha=1.5':':','alpha=1.7':'-.','alpha=1.9':(0, (3, 1))}

for alpha in [1.1,1.3,1.5,1.7,1.9]:
    name = "../build/zipf_"+str(alpha)+"_1000000.txt"
    # 读取zipf.txt文件中的数据
    with open(name, "r") as file:
        plt.hist([int(line.strip()) for line in file], bins=np.arange(0, 200000, 1000), log=True,label="alpha="+str(alpha),histtype='step',linestyle=linestyles['alpha='+str(alpha)])

# 绘制直方图，设置横坐标间隔为100

plt.legend()
# 设置坐标轴标签
plt.xlabel("Value")
plt.ylabel("Frequency (log scale)")

# 保存直方图为PNG文件，文件名与原文件名相同，但扩展名为.png
plt.savefig("../result/zipf_distribution.png")
