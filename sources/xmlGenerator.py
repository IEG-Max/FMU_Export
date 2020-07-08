import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-n', '--name', type=str, help="desired name for the FMU", required=True)
args = parser.parse_args()

path = os.path.dirname(os.path.realpath(__file__))

modelDescription = open(path + "/../modelDescription.xml", "w")
modelDescription.write("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n\
<fmiModelDescription\n\
  fmiVersion=\"2.0\"\n\
  modelName=\"" + args.name + "\"\n\
  guid=\"{31415926-5358-9793-2384-626433832795}\"\n\
  numberOfEventIndicators=\"1\">\n\
  <CoSimulation\n\
    modelIdentifier=\"" + args.name + "\"\n\
    needsExecutionTool=\"false\"\n\
    canHandleVariableCommunicationStepSize=\"true\"\n\
    canInterpolateInputs=\"false\"\n\
    maxOutputDerivativeOrder=\"0\"\n\
    canRunAsynchronuously = \"false\"\n\
    canBeInstantiatedOnlyOncePerProcess=\"false\"\n\
    canNotUseMemoryManagementFunctions=\"false\"\n\
    canGetAndSetFMUstate=\"false\"\n\
    canSerializeFMUstate=\"false\" >\n\
  </CoSimulation>\n\
  <ModelVariables>\n")

# INPUTS
inputs = open(path + "/../resources/input_ports.txt", "r")
lines = inputs.readlines()
inputs.close()
start_values={"Real": "0", "Integer": "0", "Boolean": "false", "String": ""}
i = 0
for line in lines:
  tokens = line.split("/")
  modelDescription.write("    <ScalarVariable\n\
      name=\"" + tokens[0] + "/" + tokens[1] + "/" + tokens[2] + "\"\n\
      valueReference=\"" + str(i) + "\"\n\
      causality=\"input\">\n\
      <" + tokens[3][:-1] + " start=" + "\"" + start_values[tokens[3][:-1]] + "\"/>\n\
    </ScalarVariable>\n")
  i = i + 1
initial_output_index = i + 1
# OUTPUTS
outputs = open(path + "/../resources/output_ports.txt", "r")
lines = outputs.readlines()
outputs.close()
for line in lines:
  tokens = line.split("/")
  modelDescription.write("    <ScalarVariable\n\
      name=\"" + tokens[0] + "/" + tokens[1] + "/" + tokens[2] + "\"\n\
      valueReference=\"" + str(i) + "\"\n\
      causality=\"output\">\n\
      <" + tokens[3][:-1] + "/>\n\
    </ScalarVariable>\n")
  i = i + 1

# ModelStructure
modelDescription.write("\
</ModelVariables>\n\
  <ModelStructure>\n\
    <Outputs>\n")
for j in range(initial_output_index,initial_output_index + len(lines)):
    modelDescription.write("      <Unknown index=\"" + str(j) + "\"/>\n")
modelDescription.write("\
    </Outputs>\n\
  </ModelStructure>\n\
</fmiModelDescription>")
modelDescription.close()
