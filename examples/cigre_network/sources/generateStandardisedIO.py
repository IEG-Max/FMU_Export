import pandapower as pp

# build the input_ports file

net = pp.from_json('../resources/cigre_network_lv.json')
file = open("../resources/input_ports.txt", "w")

for e in net.load['name']:
    file.write("load/"+e+"/p_mw\n")
    file.write("load/"+e+"/q_mvar\n")
file.close()

outputs = ["Bus_0", "Bus_R0", "Bus_R1", "Bus_R2", "Bus_R3", "Bus_R4", "Bus_R5", "Bus_R6", "Bus_R7",
 "Bus_R8","Bus_R9", "Bus_R10", "Bus_R11", "Bus_R12", "Bus_R13", "Bus_R14", "Bus_R15", "Bus_R16",
 "Bus_R17", "Bus_R18", "Bus_I0", "Bus_I1", "Bus_I2", "Bus_C0", "Bus_C1", "Bus_C2", "Bus_C3",
 "Bus_C4", "Bus_C5", "Bus_C6", "Bus_C7", "Bus_C8", "Bus_C9", "Bus_C10", "Bus_C11", "Bus_C12",
 "Bus_C13", "Bus_C14", "Bus_C15", "Bus_C16", "Bus_C17", "Bus_C18", "Bus_C19", "Bus_C20"]
file = open("../resources/output_ports.txt", "w")
for e in outputs:
    file.write("res_bus/"+ e + "/vm_pu\n")
file.close()
