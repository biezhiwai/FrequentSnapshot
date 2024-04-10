import pandas as pd
import matplotlib.pyplot as plt


def myplot(row_names,column_names,file_name,ylabel,save_name,scale=1,yMax=0):
    markers = {"Naive":'o',
               "COU":'s',
               "Fork":'^',
               "Zigzag":'D',
               "PingPong":'*',
               "FHC":'x',
               "FHCC":'+'}
    
    colors = {"Naive":'b',
                "COU":'g',
                "Fork":'r',
                "Zigzag":'c',
                "PingPong":'m',
                "FHC":'y',
                "FHCC":'k'}

    df = pd.read_csv(file_name,names=['Row Size[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
    df = df[column_names] / scale

    plt.figure(figsize=(8, 7))
    plt.rcParams.update({'font.size': 23})
    plt.grid(True, linestyle='--')
    if yMax != 0:
        plt.ylim(0, yMax)

    for column in df.columns[1:]: 
        plt.plot(df.index, df[column], marker=markers[column], label=column,color=colors[column])

    plt.xticks(df.index,row_names)
    plt.legend()
    plt.xlabel(df.columns[0])
    plt.ylabel(ylabel)
    plt.tight_layout()
    plt.savefig(save_name)


column_names = ['Row Size[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_ckp_overhead.csv'
ylable = 'Avg ckp overhead[s]'
save_name = '../result/avg_ckp_overhead vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000)

column_names = ['Row Size[B]',"Fork","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_prepare.csv'
ylable = 'Avg prepare[us]'
save_name = '../result/avg_prepare vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000)

column_names = ['Row Size[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_tick_latency.csv'
ylable = 'Avg tick latency[us]'
save_name = '../result/avg_tick_latency vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1,450)

column_names = ['Row Size[B]',"Fork","PingPong","FHC","FHCC"]
# column_names = ['Row Size[B]',"Fork","FHC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/max_tick_latency.csv'
ylable = 'Max tick latency[us]'
save_name = '../result/max_tick_latency vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name)