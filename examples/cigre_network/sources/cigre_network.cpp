
#include "simgrid-fmi.hpp"
#include "simgrid/s4u.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <dlfcn.h>
#include <vector>
#include <math.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(main, "Messages specific for this s4u example");

static int setter(int with_pv, std::string varName, std::string p_port, std::string q_port){
  std::vector<double> input_p;
  std::vector<double> input_q;
  std::string line;
  std::ifstream input_file;
  input_file.open("../resources/" + varName + ".txt");
  while(std::getline(input_file, line)){
    input_p.push_back(stod(line.substr(0, line.find(" ") - 1)));
    input_q.push_back(stod(line.substr(line.find(" ") + 1)));
  }
  input_file.close();
  for (int i = 0; i < input_p.size(); i++){
    if (!with_pv && varName.substr(0,2) == "PV"){
      simgrid::fmi::set_real("cigre_network", p_port, 0);
      simgrid::fmi::set_real("cigre_network", q_port, 0);
    }
    else{
      simgrid::fmi::set_real("cigre_network", p_port, input_p[i]);
      simgrid::fmi::set_real("cigre_network", q_port, input_q[i]);
    }
    simgrid::s4u::this_actor::sleep_for(1);
  }
  input_file.close();
  return 0;
}

static int getter(int with_pv, std::vector<std::string> output_ports){
  std::vector<double> vm_pu_values;
  simgrid::s4u::this_actor::sleep_for(0.5);
  for (int i = 0; i < 289; i++){
    for (auto out : output_ports) {
      vm_pu_values.push_back(simgrid::fmi::get_real("cigre_network", out));
    }
    vm_pu_values.push_back(-1);
    std::cout << i + 1 << "/" << "289" << "\n";
    simgrid::s4u::this_actor::sleep_for(1);
  }
  std::ofstream res_file;
  if (with_pv)
    res_file.open ("../resources/vm_pu_res_with_pv.txt", std::ios::trunc);
  else
    res_file.open ("../resources/vm_pu_res_without_pv.txt", std::ios::trunc);
  for (auto v : vm_pu_values){
    if (v == -1)
      res_file << "\n";
    else
      res_file << v << "\n";
  }
  res_file.close();
  return 0;
}


int main (int argc, char *argv[]){

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " PV_ON/PV_OFF\n";
    return 1;
  }

  int with_pv;
  if (!strcmp(argv[1], "PV_ON"))
    with_pv = 1;
  else if (!strcmp(argv[1], "PV_OFF"))
    with_pv = 0;
  else {
    std::cout << "B";
    std::cerr << "Usage: " << argv[0] << " PV_ON/PV_OFF: " <<
     "Got " << argv[1] << "\n";
    return 1;
  }


  // load list of inputs ports
  std::string line;
  std::vector<std::string> input_ports;
  std::ifstream input_ports_file;
  input_ports_file.open("../resources/input_ports.txt");
  while(std::getline(input_ports_file, line)) {
    int i = line.find("/", line.find("/", line.find("/") + 1) + 1);
    input_ports.push_back(line.substr(0, i));
  }
  input_ports_file.close();

  // load list of output ports
  std::vector<std::string> output_ports;
  std::ifstream output_ports_file;
  output_ports_file.open("../resources/output_ports.txt");
  while(std::getline(output_ports_file, line)) {
    int i = line.find("/", line.find("/", line.find("/") + 1) + 1);
    output_ports.push_back(line.substr(0, i));
  }
  output_ports_file.close();

  // start simgrid
  simgrid::s4u::Engine e(&argc, argv);
  e.load_platform("../platforms/one_host.xml");

  // start fmi plugin
  const double intstepsize = 0.01;
  simgrid::fmi::init(intstepsize);
  simgrid::fmi::add_fmu_cs("../cigre_network.fmu", "cigre_network");
  simgrid::fmi::ready_for_simulation();

  // start actors
  for (int i = 0; i < input_ports.size(); i = i + 2){
    int fst = input_ports[i].find('/');
    int snd = input_ports[i].find('/', input_ports[i].find('/') + 1);
    std::string varName = input_ports[i].substr(fst + 1, snd - 1 - fst);
    simgrid::s4u::Actor::create("setter_" + varName, simgrid::s4u::Host::by_name("host"), setter, with_pv, varName, input_ports[i], input_ports[i+1]);
  }
  simgrid::s4u::Actor::create("getter", simgrid::s4u::Host::by_name("host"), getter, with_pv, output_ports);
  e.run();
  return 0;
}
