from fmpy import read_model_description, extract
from fmpy.fmi2 import FMU2Slave
import shutil

# time in seconds
start_time = 0
step_size = 1
stop_time = 2

# name the fmu
fmu_filename = "TestFMU.fmu"

# read the model description
model_description = read_model_description(fmu_filename)

# collect the value references
vrs = {}
for variable in model_description.modelVariables:
    vrs[variable.name] = variable.valueReference

# extract the FMU
unzipdir = extract(fmu_filename)

fmu = FMU2Slave(guid=model_description.guid,
                unzipDirectory=unzipdir,
                modelIdentifier=model_description.coSimulation.modelIdentifier,
                instanceName='instance1')

# initialize
fmu.instantiate()
fmu.setupExperiment(startTime=start_time)
fmu.enterInitializationMode()
fmu.exitInitializationMode()

time = start_time

print(vrs)

# # set the first parameters that initilize the problem
# dict_data = {
#     'm_flow_1': [2, 1],
#     'm_flow_2': [3, 2],
# }

# # list for the outputs
# list_outputs = ['res_pipe_v_mean_m_per_s_0', ]

# # for key in dict_data:
# #     fmu.setReal([vrs[key], ], [dict_data[key][0], ])

# outputs = {}

# # simulation loop
# while time < stop_time:

#     # NOTE: the FMU.get*() and FMU.set*() functions take lists of
#     # value references as arguments and return lists of values
#     print('NEW TIME')
#     print(time)
#     print('')

#     # set the mass flow rate for every time step
#     for key in dict_data:
#         fmu.setReal([vrs[key], ], [dict_data[key][time], ])

#     # perform one step of the simulation
#     fmu.doStep(currentCommunicationPoint=time, communicationStepSize=step_size)

#     # get the inputs that has been set
#     for key in dict_data:
#         print(key + ": " + str(fmu.getReal([vrs[key], ])[0]))

#     # get the outputs after the simu step
#     outputs[time] = fmu.getReal([vrs[list_outputs[0]], ])[0]

#     ## advance the time
#     time += step_size

# fmu.terminate()
# fmu.freeInstance()

# # clean up
# shutil.rmtree(unzipdir)

# print(outputs)