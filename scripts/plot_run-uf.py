import pandas as pd
import matplotlib.pyplot as plt


def myplot(row_names,column_names,file_name,ylabel,save_name,scale=1,std_file_name=None,yMax=0,log=False):
    markers = {"Naive":'o',
               "COU":'s',
               "Fork":'^',
               "Zigzag":'D',
               "PingPong":'*',
               "FHC":'x',
               "FHCC":'v'}
    
    colors = {"Naive":'b',
                "COU":'g',
                "Fork":'r',
                "Zigzag":'c',
                "PingPong":'m',
                "FHC":'y',
                "FHCC":'k'}
    capsizes = {
        "Naive":0,
        "COU":2,
        "Fork":4,
        "Zigzag":6,
        "PingPong":8,
        "FHC":10,
        "FHCC":12
    }

    df = pd.read_csv(file_name,names=['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
    df = df[column_names] / scale

    if std_file_name is not None:
        errorBars = pd.read_csv(std_file_name,names=['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
        errorBars = errorBars[column_names] / scale

    plt.figure(figsize=(8, 7))
    plt.rcParams.update({'font.size': 23,'font.sans-serif':'Microsoft Yahei'})
    plt.grid(True, linestyle='--')
    if yMax != 0:
        plt.ylim(0, yMax)

    for column in df.columns[1:]: 
        errorBar = errorBars[column] if 'errorBars' in locals() else None
        plt.errorbar(df.index, df[column], marker=markers[column],markersize=12, label=column,color=colors[column],
                     yerr=errorBar,capsize=capsizes[column],capthick=2)

    plt.xticks(df.index,row_names)
    if log:
        plt.yscale('log')
    plt.legend()
    plt.xlabel(df.columns[0])
    plt.ylabel(ylabel)
    plt.tight_layout()
    plt.savefig(save_name)


column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
# column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong"]
row_names = ['16','32','64','128','256']
file_name = '../result/run-uf/avg_ckp_overhead.csv'
std_file_name = '../result/run-uf/std_ckp_overhead.csv'
ylable = '平均快照存储时间[s]'
save_name = '../result/avg_ckp_overhead vs uf.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000,std_file_name)

column_names = ['更新频率[K/Tick]',"Fork","PingPong","FHC","FHCC"]
# column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong"]
row_names = ['16','32','64','128','256']
file_name = '../result/run-uf/avg_prepare.csv'
std_file_name = '../result/run-uf/std_prepare.csv'
ylable = '平均快照摄取时间[μs]'
save_name = '../result/avg_prepare vs uf.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000,std_file_name)

column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
# column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong"]
row_names = ['16','32','64','128','256']
file_name = '../result/run-uf/avg_tick_latency.csv'
ylable = '平均Tick延迟[μs]'
save_name = '../result/avg_tick_latency vs uf.png'

myplot(row_names,column_names,file_name,ylable,save_name,1)

column_names = ['更新频率[K/Tick]',"Fork","PingPong","FHC","FHCC"]
# column_names = ['更新频率[K/Tick]','Naive',"COU","Fork","Zigzag","PingPong"]
row_names = ['16','32','64','128','256']
file_name = '../result/run-uf/max_tick_latency.csv'
ylable = '最大Tick延迟[ms]'
save_name = '../result/max_tick_latency vs uf.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000)