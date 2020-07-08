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
pp.create_ext_grid(net, bus=bus1, vm_pu=1.02, name="Grid_Connection")
pp.create_load(net, bus=bus3, p_mw=0.100, q_mvar=0.05, name="Load")

#create branch elements
trafo = pp.create_transformer(net, hv_bus=bus1, lv_bus=bus2, std_type="0.4 MVA 20/0.4 kV", name="Trafo")
line = pp.create_line(net, from_bus=bus2, to_bus=bus3, length_km=0.1, std_type="NAYY 4x50 SE", name="Line")

###### DATA STRUCTURE #####
print(net.bus)
print(net.line)
print(net.trafo)
print(net.load)


##### POWER FLOW #####
pp.runpp(net)
print(net.res_line)
print(net.res_trafo)

##### TAP CHANGERS #####
net.trafo.tap_pos.at[trafo] = -1
pp.runpp(net)
print(net.res_bus)

##### SWITCHES #####
pp.create_switch(net, bus=bus3, element=line, et="l", name="Switch", closed=False)
pp.runpp(net)
print(net.res_bus)
print(net.res_load)
print(net.res_line)

##### TOPOLOGICAL ANALYSIS #####
# skipped

##### SHORT CIRCUIT ANALYSIS #####
# skipped
