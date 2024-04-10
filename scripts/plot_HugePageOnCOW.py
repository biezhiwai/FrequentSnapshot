import pandas as pd
import matplotlib.pyplot as plt


def myplot():
    markers = {"4KB page":'o',
               "2MB page":'s'}
    
    colors = {"4KB page":'b',
              "2MB page":'g'}

    df1 = pd.read_csv("../result/HugePageOnCOW.csv",header=None) / 1000

    df1.columns = ["uf[Count]","2MB page","4KB page"]


    plt.figure(figsize=(7, 4))
    plt.grid(True, linestyle='--')

    # plt.ylim(0, 500)

    plt.plot(df1.index, df1["4KB page"], marker=markers["4KB page"], label="4KB page",color=colors["4KB page"])
    plt.plot(df1.index, df1["2MB page"], marker=markers["2MB page"], label="2MB page",color=colors["2MB page"])


    # plt.ylim(bottom=0)
    plt.xticks(df1.index,['16','32','64','128','256'])
    plt.yscale('log')
    plt.legend()
    plt.xlabel(df1.columns[0])
    plt.ylabel('COW time[us]')
    plt.savefig('../result/COW time vs dataSize.png')



myplot()