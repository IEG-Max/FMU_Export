
#include "simgrid-fmi.hpp"
#include "simgrid/s4u.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <dlfcn.h>
#include <vector>
#include <math.h>

XBT_LOG_NEW_DEFAULT_CATEGORY(main, "Messages specific for this s4u example");

static int actor(){
    simgrid::fmi::set_real("minimal_example", "ext_grid/Grid_Connection/vm_pu", 1.02);
    simgrid::fmi::set_real("minimal_example", "load/Load/q_mvar", 0.05);

    // test all get
    bool b = simgrid::fmi::get_boolean("minimal_example", "line/Line/in_service");
    std::cout << "in_service: " << b << "\n";
    int i = simgrid::fmi::get_integer("minimal_example", "line/Line/from_bus");
    std::cout << "from_bus: " << i << "\n";
    double r = simgrid::fmi::get_real("minimal_example", "line/Line/c_nf_per_km");
    std::cout << "c_nf_per_km: " << r << "\n";
    std::string s = simgrid::fmi::get_string("minimal_example", "line/Line/std_type");
    std::cout << "std_type: " << s << "\n";


    return 0;
}

int main (int argc, char *argv[]){

  // start simgrid
  simgrid::s4u::Engine e(&argc, argv);
  e.load_platform("../platforms/one_host.xml");

  // start fmi plugin
  const double intstepsize = 0.01;
  simgrid::fmi::init(intstepsize);
  simgrid::fmi::add_fmu_cs("../minimal_example.fmu", "minimal_example");
  simgrid::fmi::ready_for_simulation();

  // start actor
  simgrid::s4u::Actor::create("actor", simgrid::s4u::Host::by_name("host"), actor);
  e.run();
  return 0;
}
