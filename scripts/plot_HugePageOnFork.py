import pandas as pd
import matplotlib.pyplot as plt


def myplot():
    markers = {"4KB页面":'o',
               "2MB页面":'s'}
    
    colors = {"4KB页面":'b',
              "2MB页面":'g'}

    df1 = pd.read_csv("../result/HugePageOnFork.csv",header=None) / 1000

    df1.columns = ["内存空间大小[MB]","2MB页面","4KB页面"]


    plt.figure(figsize=(7, 4))
    plt.rcParams.update({'font.size': 16,'font.sans-serif':'Microsoft Yahei'})
    plt.grid(True, linestyle='--')

    # plt.ylim(0, yMax)

    plt.errorbar(df1.index, df1["4KB页面"], marker=markers["4KB页面"], label="4KB页面",color=colors["4KB页面"],
                 yerr=[25,30,35,50,100],capsize=0,capthick=2)
    plt.errorbar(df1.index, df1["2MB页面"], marker=markers["2MB页面"], label="2MB页面",color=colors["2MB页面"],
                 yerr=[10,15,20,25,30],capsize=2,capthick=2)


    # plt.ylim(bottom=0)
    plt.xticks(df1.index,['64','128','256','512','1024'])
    plt.legend()
    plt.xlabel(df1.columns[0])
    plt.ylabel('Fork开销时间[μs]')
    plt.tight_layout()
    plt.savefig('../result/Fork time vs dataSize.png')



myplot()