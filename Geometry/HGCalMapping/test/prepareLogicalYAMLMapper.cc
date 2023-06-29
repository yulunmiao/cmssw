#include "Geometry/HGCalMapping/interface/HGCalModuleLocator.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include <TString.h>
#include <yaml-cpp/yaml.h>
#include <iostream>

int main(int argc, char** argv) {

    if (argc<1) {
        std::cout << "Usage: prepareLogicalYAMLMapper modulelocatorfile" << std::endl;
        return -1;
    }
    std::string modulemap(argv[1]);

    HGCalModuleLocator modloc;
    modloc.buildLocatorFrom(modulemap,true);
    auto &info = modloc.getInfo();

    //build YAML
    std::ofstream outf;
    outf.open("cfg_mapper.yaml", std::ios::app);

    for(auto m : info.params_) {
      uint32_t elecid = HGCalElectronicsId(m.zside,m.fedid,m.captureblock,m.econdidx,0,0).raw();
      std::map<std::string,std::string> moduleCfg;
      moduleCfg["ECON"] = "pedestal_run0.yaml";
      moduleCfg["ROCs"] = "initial_full_config.yaml";
      YAML::Emitter newNode;
      newNode << YAML::BeginMap;
      newNode << YAML::Key << Form("id: 0x%x",elecid);
      newNode << YAML::Value << YAML::BeginMap << YAML::Key << "configs" << YAML::Value << moduleCfg << YAML::EndMap;
      newNode << YAML::EndMap;
      outf << newNode.c_str() << "\n";
    }

    outf.close();
        
    return 0;
}
