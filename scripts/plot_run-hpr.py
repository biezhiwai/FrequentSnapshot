import pandas as pd
import matplotlib.pyplot as plt


def myplot(row_names,column_names,file_name,ylabel,save_name,scale=1,yMax=0):
    markers = {"Naive":'o',
               "COU":'s',
               "Fork":'^',
               "Zigzag":'D',
               "PingPong":'*',
               "Fork-hot/cold":'x',
               "Fork-hot/cold+cou":'+'}
    
    colors = {"Naive":'b',
                "COU":'g',
                "Fork":'r',
                "Zigzag":'c',
                "PingPong":'m',
                "Fork-hot/cold":'y',
                "Fork-hot/cold+cou":'k'}

    df = pd.read_csv(file_name,names=['Huge page ratio',"Fork","Fork-hot/cold","Fork-hot/cold+cou"],header=None)
    df = df[column_names] / scale

    plt.figure(figsize=(7, 4))
    plt.grid(True, linestyle='--')

    if yMax != 0:
        plt.ylim(0, yMax)

    for column in df.columns[1:]: 
        plt.plot(df.index, df[column], marker=markers[column], label=column,color=colors[column])

    plt.ylim(bottom=0)
    plt.xticks(df.index,row_names)
    plt.legend()
    plt.xlabel(df.columns[0])
    plt.ylabel(ylabel)
    plt.savefig(save_name)

column_names = ['Huge page ratio',"Fork","Fork-hot/cold","Fork-hot/cold+cou"]
row_names = ['0','0.3','0.5','0.7','0.9','0.95','0.99','1']
file_name = '../result/run-hpr/avg_ckp_overhead.csv'
ylable = 'Avg ckp overhead[ms]'
save_name = '../result/avg_ckp_overhead vs hpr.png'

myplot(row_names,column_names,file_name,ylable,save_name)

column_names = ['Huge page ratio',"Fork","Fork-hot/cold","Fork-hot/cold+cou"]
row_names = ['0','0.3','0.5','0.7','0.9','0.95','0.99','1']
file_name = '../result/run-hpr/avg_prepare.csv'
ylable = 'Avg prepare[us]'
save_name = '../result/avg_prepare vs hpr.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000)

column_names = ['Huge page ratio',"Fork","Fork-hot/cold","Fork-hot/cold+cou"]
row_names = ['0','0.3','0.5','0.7','0.9','0.95','0.99','1']
file_name = '../result/run-hpr/avg_tick_latency.csv'
ylable = 'Avg tick latency[us]'
save_name = '../result/avg_tick_latency vs hpr.png'

myplot(row_names,column_names,file_name,ylable,save_name)

column_names = ['Huge page ratio',"Fork","Fork-hot/cold","Fork-hot/cold+cou"]
row_names = ['0','0.3','0.5','0.7','0.9','0.95','0.99','1']
file_name = '../result/run-hpr/max_tick_latency.csv'
ylable = 'Max tick latency[us]'
save_name = '../result/max_tick_latency vs hpr.png'

myplot(row_names,column_names,file_name,ylable,save_name)