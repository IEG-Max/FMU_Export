f = open("logfile_input.txt")
lines = f.readlines()
time_dict = dict() # dict of dict : {time: {input: value}}
# build dict of input new values, in a dict of time at which they are modified
for line in lines:
    tokens = line.split()
    time_dict[tokens[1]] = time_dict.get(tokens[1],dict())
    time_dict[tokens[1]][tokens[0]] = tokens[2]
f.close()

# write the input file headers
f = open("modified_input.txt", "w+")
f.write("time")
for i in range(1,56):
    f.write(",load/LOAD" + str(i) + "/p_mw")
f.write("\n")

# build the dict of last kown values of inputs (0 for now)
last_value = dict()
for i in range(1,56):
    last_value["load/LOAD" + str(i) + "/p_mw"] = 0

# write the input file according to the syntaxe of the FMI cross check
for k,v in time_dict.items():
    f.write(str(k))
    for i in range(1,56):
        var_name = "load/LOAD" + str(i) + "/p_mw"
        last_value[var_name] = v.get(var_name, last_value[var_name])
        f.write("," + last_value[var_name])
    f.write("\n")
