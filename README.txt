***** PANDAPOWER FMU *****


**********************
* HOW TO COMPILE FMU *
**********************

go to build:
$ cmake . && make


**********************
* BUILDING YOUR FMU *
**********************

+ at the root of the project run the command:
$python3 makeFMU.py

+ The command will ask you for a name for the fmu and to add paths to input_ports file, output_port file and network file:
$python3 makeFMU.py -n NAME -i INPUT -o OUTPUT -nw NETWORK

**********************
* FILES DESCRIPTIION *
**********************
some files are required to compile the FMU:

+ input_ports.txt
defines the list of input ports in your FMU as:

port1
port2
...

ports must follow this syntax:
dataframe/variable/column/type

"dataframe" is a dataframe of the pandapower API (load, bus...)
"variable" is the name of the variable in the dataframe, necessary to
  find the row you want to set/get, it must not contain a "/"
"column" is the attribute to set/get in the row
"type" is the type of the variable in pandapower, it must be one of the following:
   Real Integer Boolean String

+ output_ports.txt
defines the list of output ports in your FMU similarly to the input ports

+ network.json
defines the network that will be loaded by pandapower,
it is advised to build it using the pandapower module through python

+ simgrid and simgrid-fmi must be installed on the machine to run the examples
https://simgrid.org/
https://framagit.org/simgrid/simgrid-FMI
