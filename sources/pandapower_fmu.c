#define MODEL_IDENTIFIER pandapower
#include "python3.7/Python.h"
#include "libxml/parser.h"
#include "fmi2Functions.h"
#include <dlfcn.h>

fmi2Real sim_time;
int modified_state;

PyObject *pandapower;
PyObject *net;
xmlDoc *xmlDescription;

void updatePandapower(void){
    if (modified_state){
        PyObject * run = PyObject_CallMethodObjArgs(pandapower, Py_BuildValue("s", "runpp"), net, NULL);
        if(run==NULL){
            printf("Error while running load flow of pandapower network.\n");
            if(PyErr_Occurred()) PyErr_Print();
        }
        modified_state = 0;
    }
}

fmi2String GetVarName(fmi2Integer value){
    xmlNode *cur;
    cur = xmlDocGetRootElement(xmlDescription);
    if (cur == NULL){
        printf("unable to find the root of modelDescription.xml.\n");
    }
    cur = cur->children->next;
    while (cur != NULL &&
        (xmlStrcmp(cur->name, (const xmlChar *) "ModelVariables") != 0)){
            cur = cur->next->next;
    }
    cur = cur->xmlChildrenNode->next;
    xmlChar valueReference[10];
    sprintf(valueReference, "%d", value);
    while (cur != NULL &&
        (xmlStrcmp(xmlGetProp(cur, "valueReference"), (const xmlChar*) valueReference) != 0)){
        cur = cur->next->next;
    }
    char * r = xmlGetProp(cur, "name");
    xmlCleanupParser();
    xmlMemoryDump();
    return r;
}

int getIndex(fmi2String dataframe, fmi2String varName){
    // net.dataframe
    PyObject *dataframe_pp;

    char absolute_dataframe[strlen(dataframe) - 3];
    int res_dataframe = 0;
    if (dataframe[0] == 'r' && dataframe[1] == 'e' && dataframe[2] == 's') {
        res_dataframe = 1;
        for (int i = 0; i < strlen(dataframe) - 4; i++) {
            absolute_dataframe[i] = dataframe[i+4];
        }
        absolute_dataframe[strlen(dataframe) - 4] = '\0';
        // -- res_dataframe do not contain variable names, we search the index in the dataframe
        dataframe_pp = PyObject_GetAttrString(net, absolute_dataframe);
    }
    else
        dataframe_pp = PyObject_GetAttrString(net, dataframe);

    if(dataframe_pp==NULL){
        printf("Error while loading %s extension of network.\n", res_dataframe? absolute_dataframe : dataframe);
        if(PyErr_Occurred()) PyErr_Print();
    }

    // net.dataframe["name"] == varName
    PyObject *bool_col = PyObject_RichCompare(PyObject_GetItem(dataframe_pp, Py_BuildValue("s", "name")) , Py_BuildValue("s", varName), Py_EQ);
    if(PyErr_Occurred()) PyErr_Print();

    // net.dataframe[net.dataframe["name"] == varName]
    PyObject * row = PyObject_GetItem(dataframe_pp, bool_col);
    if(PyErr_Occurred()) PyErr_Print();

    // net.dataframe[net.dataframe["name"] == varName].index
    PyObject *index_py = PyObject_GetAttrString(row, "index");
    if(PyErr_Occurred()) PyErr_Print();

    // net.dataframe[net.dataframe["name"] == varName].index.values[0]
    PyObject *index_py_int = PyObject_GetItem(PyObject_GetAttrString(index_py, "values"), Py_BuildValue("i", 0));
    if(PyErr_Occurred()) PyErr_Print();

    // get the index of varName in the dataframe
    int index = PyLong_AsLong(index_py_int);
    if(PyErr_Occurred()) PyErr_Print();

    return index;
}

/****************************************************
Common Functions
****************************************************/

const char* fmi2GetTypesPlatform(void){
    return "default";
}

const char* fmi2GetVersion(void){
    return "2.0";
}

fmi2Status  fmi2SetDebugLogging(fmi2Component c, fmi2Boolean loggingOn, size_t nCategories, const fmi2String categories[]){
    // NOT IMPLEMENTED
    return fmi2OK;
}

fmi2Component fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String fmuGUID, fmi2String fmuResourceLocation, const fmi2CallbackFunctions* functions, fmi2Boolean visible, fmi2Boolean loggingOn){
    FILE *file ;
    file = popen("python3 --version", "r");
    char line[50];
    char path[100] = "/usr/lib/python3.7/config-3.7m-x86_64-linux-gnu/libpython3.7.so";
    if (fgets(line, 50, file) == NULL)
        printf("Error: No version of Python3 found.");
    else {
        path[17] = line[9];
        path[28] = line[9];
        path[59] = line[9];
    }

    dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    Py_Initialize();
    modified_state = 1;

    // importing pandapower module
    pandapower = PyImport_Import(PyUnicode_FromString("pandapower"));
    if(pandapower==NULL){
        printf("Error while loading module pandapower.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    // importing from_json function
    PyObject *from_json;
    from_json = PyObject_GetAttrString(pandapower, "from_json");
    if(from_json==NULL){
        printf("Error while loading from_json function of pandapower.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    // using from_json
    char s[1024];
    char ss[1024];
    strcpy(s, fmuResourceLocation);
    memmove(s, s + 7, strlen(s));
    strcat(s, "/network.json");
    PyObject *arg = Py_BuildValue("s", s);
    if(arg==NULL){
        printf("Error while building path to json file.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }
    net = PyObject_CallFunctionObjArgs(from_json, arg, NULL);
    if(net==NULL){
        printf("Error while loading network of pandapower.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    char model_path[1024];
    strcpy(model_path, fmuResourceLocation);
    strcat(model_path, "/../modelDescription.xml");
    xmlDescription = xmlParseFile(model_path);
    if (xmlDescription == NULL){
        printf("modelDescription.xml not found.\n");
    }

    return net;
}

void fmi2FreeInstance(fmi2Component c){
}

fmi2Status fmi2SetupExperiment(fmi2Component c, fmi2Boolean toleranceDefined, fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime){
    sim_time = startTime;
    return fmi2OK;
}

fmi2Status fmi2EnterInitializationMode(fmi2Component c){
    // NOT IMPLEMENTED
    return fmi2OK;
}

fmi2Status fmi2ExitInitializationMode(fmi2Component c){
    // NOT IMPLEMENTED
    return fmi2OK;
}

fmi2Status fmi2Terminate(fmi2Component c){
    Py_Finalize();
    return fmi2OK;
}

fmi2Status fmi2Reset(fmi2Component c){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]){
    updatePandapower();
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col]
    PyObject * res = PyObject_GetItem(at, Py_BuildValue("i,s", index, col));
    if(res==NULL){
        printf("Error while getting value.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    value[0] = PyFloat_AsDouble(res);
    return fmi2OK;
}

fmi2Status fmi2GetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]){
    updatePandapower();
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col]
    PyObject * res = PyObject_GetItem(at, Py_BuildValue("i,s", index, col));
    if(res==NULL){
        printf("Error while getting value.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    value[0] = (int) PyLong_AsLong(res);
    return fmi2OK;
}

fmi2Status fmi2GetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]){
    updatePandapower();
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col]
    PyObject * res = PyObject_GetItem(at, Py_BuildValue("i,s", index, col));
    if(res==NULL){
        printf("Error while getting value.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    value[0] = (int) PyLong_AsLong(res) ? fmi2True : fmi2False;
    return fmi2OK;
}

fmi2Status fmi2GetString (fmi2Component c, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]){
    updatePandapower();
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col]
    PyObject * res = PyObject_GetItem(at, Py_BuildValue("i,s", index, col));
    if(res==NULL){
        printf("Error while getting value.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    value[0] = PyUnicode_AsUTF8(res);
    return fmi2OK;
}

/* Build a python command to Set a Real in the pandapower network
*
* Search in a dataframe "dataframe" of the network for a variable with name "varName" (a row of the dataframe)
* and set the value of column "col" to the desired value
*
* note: it assumes that loads are in the same order in the network.json and modelDescription.xml
*/
 fmi2Status fmi2SetReal(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]){
     fmi2String varStdName = GetVarName(vr[0]);
     fmi2String dataframe = strsep((char**)&varStdName,"/");
     fmi2String varName = strsep((char**)&varStdName,"/");
     fmi2String col = strsep((char**)&varStdName,"/");

     //net.dataframe
     PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

     // net.dataframe.at
     PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
     if(at==NULL){
         printf("Error while loading indexer \"at\" extension of the load.\n");
         if(PyErr_Occurred()) PyErr_Print();
     }

     int index = getIndex(dataframe, varName);

     //net.dataframe.at[index, col] = value
     PyObject_SetItem(at, Py_BuildValue("i,s", index, col), Py_BuildValue("d", value[0]));
     if(PyErr_Occurred()) PyErr_Print();

     modified_state = 1;
     return fmi2OK;
 }

fmi2Status fmi2SetInteger(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]){
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col] = value
    PyObject_SetItem(at, Py_BuildValue("i,s", index, col), Py_BuildValue("i", value[0]));
    if(PyErr_Occurred()) PyErr_Print();

    modified_state = 1;
    return fmi2OK;
}

fmi2Status fmi2SetBoolean(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]){
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col] = value
    PyObject_SetItem(at, Py_BuildValue("i,s", index, col), Py_BuildValue("p", value[0]));
    if(PyErr_Occurred()) PyErr_Print();

    modified_state = 1;
    return fmi2OK;
}

fmi2Status fmi2SetString(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]){
    fmi2String varStdName = GetVarName(vr[0]);
    fmi2String dataframe = strsep((char**)&varStdName,"/");
    fmi2String varName = strsep((char**)&varStdName,"/");
    fmi2String col = strsep((char**)&varStdName,"/");

    //net.dataframe
    PyObject * dataframe_pp = PyObject_GetAttrString(net, dataframe);

    // net.dataframe.at
    PyObject *at = PyObject_GetAttrString(dataframe_pp, "at");
    if(at==NULL){
        printf("Error while loading indexer \"at\" extension of the load.\n");
        if(PyErr_Occurred()) PyErr_Print();
    }

    int index = getIndex(dataframe, varName);

    //net.dataframe.at[index, col] = value
    PyObject_SetItem(at, Py_BuildValue("i,s", index, col), Py_BuildValue("s", value[0]));
    if(PyErr_Occurred()) PyErr_Print();

    modified_state = 1;
    return fmi2OK;
}

fmi2Status fmi2GetFMUstate(fmi2Component c, fmi2FMUstate* FMUstate){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2SetFMUstate(fmi2Component c, fmi2FMUstate FMUstate){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2FreeFMUstate(fmi2Component c, fmi2FMUstate* FMUstate){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2SerializedFMUstateSize(fmi2Component c, fmi2FMUstate FMUstate, size_t *size){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2SerializeFMUstate(fmi2Component c, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t size){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2DeSerializeFMUstate(fmi2Component c, const fmi2Byte serializedState[], size_t size, fmi2FMUstate* FMUstate){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetDirectionalDerivative(fmi2Component c, const fmi2ValueReference vUnknown_ref[], size_t nUnknown,
    const fmi2ValueReference vKnown_ref[], size_t nknown,
    const fmi2Real dvKnown[], fmi2Real dvUnknown[]){
        // NOT IMPLEMENTED
        return fmi2Error;
    }

/***************************************************
Functions for FMI2 for Co-Simulation
****************************************************/

fmi2Status fmi2SetRealInputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], const fmi2Real value[]){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetRealOutputDerivatives(fmi2Component c, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer order[], fmi2Real value[]){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2DoStep(fmi2Component c, fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize, fmi2Boolean noSetFMUStatePriorToCurrentPoint){
    if (currentCommunicationPoint != sim_time){
        return fmi2Error;
    }
    else {
        sim_time = sim_time + communicationStepSize;
        return fmi2OK;
    }
}

fmi2Status fmi2CancelStep(fmi2Component c){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetStatus(fmi2Component c, const fmi2StatusKind s, fmi2Status* value){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetRealStatus(fmi2Component c, const fmi2StatusKind s, fmi2Real* value){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetIntegerStatus(fmi2Component c, const fmi2StatusKind s, fmi2Integer* value){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetBooleanStatus(fmi2Component c, const fmi2StatusKind s, fmi2Boolean* value){
    // NOT IMPLEMENTED
    return fmi2Error;
}

fmi2Status fmi2GetStringStatus(fmi2Component c, const fmi2StatusKind s, fmi2String* value){
    // NOT IMPLEMENTED
    return fmi2Error;
}
