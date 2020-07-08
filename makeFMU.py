import argparse
import os
import shutil
import subprocess

# delete all the files
cwd = os.getcwd()
print(os.path.join(cwd, "resources"))
if os.path.exists("modelDescription.xml"):
    os.remove("modelDescription.xml")
if os.path.exists("TestFMU.fmu"):
    os.remove("TestFMU.fmu")
if os.path.exists(os.path.join(cwd, "resources")):
    shutil.rmtree(os.path.join(cwd, "resources"), ignore_errors=True)

# Parsing arguments
parser = argparse.ArgumentParser()
parser.add_argument('-n', '--name', type=str, help="desired name for the FMU", required=True)
parser.add_argument('-i', '--input', type=str, help="path to the file containing inputs ports of the FMU", required=True)
parser.add_argument('-o', '--output', type=str, help="path to the file containing outputs ports of the FMU", required=True)
parser.add_argument('-nw', '--network', type=str, help="path to the json network file for the FMU", required=True)
args = parser.parse_args()

# Building binaries
os.system(r"cd build && cmake . && make && cd .. -DLIBXML2_INCLUDE_DIR:PATH=C:\Users\max02865\01_Arbeit\01_Fraunhofer\08_Projekte\CINES\PandaPower\pandapower-fmu\venv\Lib\site-packages\lxml\includes")

# Copying files in resources folder
current_directory = os.getcwd()
# os.system("mkdir -p resources")
os.mkdir(os.path.join(current_directory, "resources"))
ressource_folder = os.path.join(current_directory, "resources")
shutil.copy2("input_ports.txt", os.path.join(ressource_folder, "input_ports.txt"))
shutil.copy2("output_ports.txt", os.path.join(ressource_folder, "output_ports.txt"))
shutil.copy2("network.json", os.path.join(ressource_folder, "network.json"))
# os.system("cp " + args.input + " resources/input_ports.txt")
# os.system("cp " + args.output + " resources/output_ports.txt")
# os.system("cp " + args.network + " resources/network.json")

# Building xml file
os.system(r"py sources\xmlGenerator.py -n " + args.name)

# Zipping FMU
os.system("mv binaries/linux64/pandapower.so binaries/linux64/" + args.name + ".so")
os.system("zip -r "+ args.name + ".fmu binaries resources modelDescription.xml")