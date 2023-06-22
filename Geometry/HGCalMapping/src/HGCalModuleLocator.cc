#include "Geometry/HGCalMapping/interface/HGCalModuleLocator.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Utilities/interface/Exception.h"

//
void HGCalModuleLocator::buildLocatorFrom(std::string path,bool usefip)
{
  if(usefip) {
    edm::FileInPath fip(path);
    path=fip.fullPath();
  }
  std::ifstream file(path);

  std::string line;
  while(std::getline(file, line))
  {
    std::istringstream stream(line);
    int zside;
    HGCalModuleInfo m;
    stream >> m.plane >> m.u >> m.v >> m.isSiPM >> m.isHD >> m.modtype >> m.econdidx >> m.captureblock >> m.slink >> m.captureblockidx >> m.fedid >> m.DAQ >> zside;
    m.zside = (zside>0);
    mod2loc_.addParameter(m);
  }
}

