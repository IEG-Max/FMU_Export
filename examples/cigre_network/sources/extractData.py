import pandapower as pp
import pandas as pd

# build input data files from file curves.csv

net = pp.from_json('../resources/cigre_network_lv.json')
curves = pd.read_csv('../resources/curves.csv')
keys = curves.keys().values[1:]
for e in keys:
  if e[-1] == 'P':
    file = open("../resources/" + e[:-2] + ".txt", "w")
    for index,row in curves.iterrows():
      file.write(str(format(row[e]/1000,".20f")) + " " + str(format(row[e[:-1] + "Q"]/1000,".20f")) + "\n")
