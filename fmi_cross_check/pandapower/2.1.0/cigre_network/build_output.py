f = open("logfile_output.txt")
lines = f.readlines()
time_dict = dict() # dict of dict : {time: {input: value}}
last_value = dict() # dict of last kown values of inputs (0 for now)
# build dict of input new values, in a dict of time at which they are modified
for line in lines:
    tokens = line.split()
    last_value[tokens[0]] = last_value.get(tokens[0], 0)
    time_dict[tokens[1]] = time_dict.get(tokens[1],dict())
    time_dict[tokens[1]][tokens[0]] = tokens[2]
f.close()

# write the input file headers
f = open("cigre_network_ref.csv", "w+")
f.write("time")
for k,v in last_value.items():
    f.write("," + k)
f.write("\n")

# write the input file according to the syntaxe of the FMI cross check
for k,v in time_dict.items():
    f.write(str(k))
    for k2,v2 in last_value.items():
        last_value[k2] = v.get(k2, last_value[k2])
        f.write("," + last_value[k2])
    f.write("\n")
