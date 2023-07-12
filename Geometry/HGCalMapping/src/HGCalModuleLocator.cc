#include "Geometry/HGCalMapping/interface/HGCalModuleLocator.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/Utilities/interface/Exception.h"

//
void HGCalModuleLocator::buildModuleLocatorFrom(std::string path,bool usefip)
{
  if(usefip) {
    edm::FileInPath fip(path);
    path=fip.fullPath();
  }
  std::ifstream file(path);

  std::string line;
  size_t iline(0);
  while(std::getline(file, line))
  {
    iline++;
    if(iline==1) continue;
    std::istringstream stream(line);
    int zside;
    std::string itype;
    HGCalModuleInfo m;
    stream >> m.plane >> m.u >> m.v >> m.isSiPM >> m.isHD >> itype >> m.econdidx >> m.captureblock >> m.slink >> m.captureblockidx >> m.fedid >> zside;
    m.zside = (zside>0);
    m.wafType = (m.isSiPM ? 0 : std::stoi(itype));
    getInfo().addParameter(m);
  }
}
