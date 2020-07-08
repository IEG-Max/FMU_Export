import pandapower as pp
import pandas as pd

pd.set_option('display.max_columns', 30)
##### CREATING A POWER SYSTEM
#create empty net
net = pp.create_empty_network()

#create buses
bus1 = pp.create_bus(net, vn_kv=20., name="Bus 1")
bus2 = pp.create_bus(net, vn_kv=0.4, name="Bus 2")
bus3 = pp.create_bus(net, vn_kv=0.4, name="Bus 3")

#create bus elements
pp.create_ext_grid(net, bus=bus1, name="Grid_Connection")
pp.create_load(net, bus=bus3, p_mw=0.100, name="Load")

#create branch elements
trafo = pp.create_transformer(net, hv_bus=bus1, lv_bus=bus2, std_type="0.4 MVA 20/0.4 kV", name="Trafo")
line = pp.create_line(net, from_bus=bus2, to_bus=bus3, length_km=0.1, std_type="NAYY 4x50 SE", name="Line")


##### SWITCHES #####
pp.create_switch(net, bus=bus3, element=line, et="l", name="Switch")


pp.to_json(net, "network.json")

input = open("input_ports.txt", "w")
input.write("ext_grid/Grid_Connection/vm_pu/Real\n")
input.write("load/Load/q_mvar/Real\n")
input.close()

output = open("output_ports.txt", "w")
output.write("line/Line/in_service/Boolean\n")
output.write("line/Line/from_bus/Integer\n")
output.write("line/Line/c_nf_per_km/Real\n")
output.write("line/Line/std_type/String\n")
output.close()
