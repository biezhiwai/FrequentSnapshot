import matplotlib.pyplot as plt
import numpy as np

# 示例数据
x = np.array([1, 2, 3, 4, 5])
y = np.array([2, 3, 5, 7, 11])
yerr = np.array([0.2, 0.3, 0.2, 0.4, 0.5])  # 假设的误差数据（例如标准差）

# 使用 errorbar 函数绘制折线图并添加误差线
plt.errorbar(x, y, yerr=yerr, label='Data', fmt='-o', ecolor='red', capsize=5, linestyle='-', marker='o', color='blue')

# 添加标题和轴标签
plt.title('Line Plot with Error Bars')
plt.xlabel('X Axis')
plt.ylabel('Y Axis')

# 添加图例
plt.legend()

# 显示图表
plt.show()
