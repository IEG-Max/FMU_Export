import pandas as pd
import numpy as np
from matplotlib import pyplot as plt

# generate the graph to visualize results, the place is placed in resources folder
# before generating the graph run the executable both with PV_ON and PV_OFF

curves = pd.read_csv('../resources/curves.csv')

file = open("../resources/vm_pu_res_with_pv.txt", "r")
liste = list()
voltages_with_pv = list()
for line in file.readlines():
  if line == "\n":
    voltages_with_pv.append(liste)
    liste = list()
  else:
    liste.append(float(line[:-1]))
file.close()

file = open("../resources/vm_pu_res_without_pv.txt", "r")
voltages_without_pv = list()
for line in file.readlines():
  if line == "\n":
    voltages_without_pv.append(liste)
    liste = list()
  else:
    liste.append(float(line[:-1]))
file.close()

figure = plt.figure()
ax1 = plt.subplot(2, 1, 1)
ax1.boxplot(voltages_without_pv)
ax1.plot([curves.index[0], curves.index[-1]], [1.05, 1.05], '--r')
ax1.set_xticks([])
ax1.set_ylabel('voltage value (p.u.)')
plt.title('Without PV')

ax2 = plt.subplot(2, 1, 2)
ax2.boxplot(voltages_with_pv)
ax2.plot([curves.index[0], curves.index[-1]], [1.05, 1.05], '--r')
ax2.set_xticks([])
ax2.set_xlabel('timestamp (1 sample = 10 minutes)')
ax2.set_ylabel('voltage value (p.u.)')
plt.title('With PV')

# ~ plt.show()
figure.savefig('../resources/boxplots.pdf')
