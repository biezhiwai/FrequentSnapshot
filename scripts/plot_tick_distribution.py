import pandas as pd
import matplotlib.pyplot as plt


def myplot(column_names,file_name,ylabel,save_name,scale=1,yMax=0):
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

    df = pd.read_csv(file_name,names=['Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
    df = df[column_names] / scale
    df = df[1000:]

    plt.figure(figsize=(7, 4))
    plt.grid(True, linestyle='--')
    if yMax != 0:
        plt.ylim(0, yMax)

    for column in df.columns: 
        plt.plot(df.index, df[column],  label=column,color=colors[column],marker=markers[column])

    # plt.yscale('log')
    plt.legend()
    plt.xlabel("Tick[10ms]")
    plt.ylabel(ylabel)
    plt.savefig(save_name)

column_names = ['Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
file_name = '../result/tick_distribution.csv'
ylable = 'Tick latency[us]'
save_name = '../result/tick_distribution.png'

myplot(column_names,file_name,ylable,save_name)
