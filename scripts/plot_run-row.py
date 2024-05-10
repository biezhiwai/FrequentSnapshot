import pandas as pd
import matplotlib.pyplot as plt

# 横坐标刻度（csv文件的行名），图例（csv文件的列名），文件名，纵坐标名，保存文件名，缩放比例，标准差文件名，y轴最大刻度值
def myplot(row_names,column_names,file_name,ylabel,save_name,scale=1,std_file_name=None,yMax=0):
    # 不同图例的标记形状
    markers = {"Naive":'o',
               "COU":'s',
               "Fork":'^',
               "Zigzag":'D',
               "PingPong":'*',
               "FHC":'x',
               "FHCC":'v'}
    # 不同图例的颜色
    colors = {"Naive":'b',
                "COU":'g',
                "Fork":'r',
                "Zigzag":'c',
                "PingPong":'m',
                "FHC":'y',
                "FHCC":'k'}
    # 不同图例的误差条顶端冒条宽度
    capsizes = {
        "Naive":0,
        "COU":2,
        "Fork":4,
        "Zigzag":6,
        "PingPong":8,
        "FHC":10,
        "FHCC":12
    }
    # 读取csv文件并缩放
    df = pd.read_csv(file_name,names=['行大小[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
    df = df[column_names] / scale

    # 读取标准差文件并缩放
    if std_file_name is not None:
        errorBars = pd.read_csv(std_file_name,names=['行大小[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"],header=None)
        errorBars = errorBars[column_names] / scale

    # 设置图像大小和字体大小
    plt.figure(figsize=(8, 7))
    plt.rcParams.update({'font.size': 23,'font.sans-serif':'Microsoft Yahei'})
    # 设置网格线
    plt.grid(True, linestyle='--')
    # 设置y轴最大刻度值
    if yMax != 0:
        plt.ylim(0, yMax)

    # 画图
    for column in df.columns[1:]: 
        errorBar = errorBars[column] if 'errorBars' in locals() else None
        plt.errorbar(df.index, df[column], marker=markers[column],markersize=12, label=column,color=colors[column],
                      yerr=errorBar,capsize=capsizes[column],capthick=2)

    # 设置横坐标刻度和图例
    plt.xticks(df.index,row_names)
    plt.legend()
    # 设置横纵坐标名
    plt.xlabel(df.columns[0])
    plt.ylabel(ylabel)
    # 自动调整图位置
    plt.tight_layout()
    plt.savefig(save_name)


column_names = ['行大小[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_ckp_overhead.csv'
std_file_name = '../result/run-row/std_ckp_overhead.csv'
ylable = '平均快照存储时间[s]'
save_name = '../result/avg_ckp_overhead vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000,std_file_name)

column_names = ['行大小[B]',"Fork","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_prepare.csv'
std_file_name = '../result/run-row/std_prepare.csv'
ylable = '平均快照摄取时间[μs]'
save_name = '../result/avg_prepare vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000,std_file_name)

column_names = ['行大小[B]','Naive',"COU","Fork","Zigzag","PingPong","FHC","FHCC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/avg_tick_latency.csv'
ylable = '平均Tick延迟[μs]'
save_name = '../result/avg_tick_latency vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1,None,450)

column_names = ['行大小[B]',"Fork","PingPong","FHC","FHCC"]
# column_names = ['行大小[B]',"Fork","FHC"]
row_names = ['64','128','256','512','1024']
file_name = '../result/run-row/max_tick_latency.csv'
ylable = '最大Tick延迟[ms]'
save_name = '../result/max_tick_latency vs RowSize.png'

myplot(row_names,column_names,file_name,ylable,save_name,1000)