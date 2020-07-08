f = open("european_lv_ref.csv")
g = open("input_modified.txt", "w+")

lines = f.readlines()
g.write(lines[0])
for i in range(1,len(lines)-1):
    tokens = lines[i].split(",")
    tokens_next = lines[i+1].split(",")
    if tokens[0] != tokens_next[0]:
        g.write(lines[i])
    if i == len(lines) - 2 :
        g.write(lines[i+1])
