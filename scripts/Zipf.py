import numpy as np
import sys 



def generate_zipf(a, size, max_value, seed=None):
    if seed is not None:
        np.random.seed(seed)
    data = np.random.zipf(a, size)
    filtered_data = data[data <= max_value]
    while len(filtered_data) < size:
        additional_data = np.random.zipf(a, size - len(filtered_data))
        filtered_data = np.concatenate((filtered_data, additional_data[additional_data <= max_value]))
    return filtered_data

# 设置参数
a = 1.9
size = 32000
max_value = 1000000
seed = 42  # 设置随机数生成器的种子
fileName = "zipf" + "_" + str(a) + "_" + str(max_value) + ".txt"


zipfGen = generate_zipf(a, size, max_value, seed)
with open(fileName, 'w') as zipfFile:
    for i in zipfGen:
        zipfFile.write(f"{i}\n")


