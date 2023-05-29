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
    HGCalModuleInfo m;
    stream >> m.plane >> m.u >> m.v >> m.isSiPM >> m.econdidx >> m.captureblock >> m.slink >> m.captureblockidx >> m.fedid >> m.DAQ;
    mod2loc_.addParameter(m);
  }
}

HGCalModuleInfo HGCalModuleLocator::getModule(int econdidx, int captureblockidx, int fedid) const
{
  auto _electronicsMatch = [econdidx, captureblockidx, fedid](HGCalModuleInfo m){ 
    return m.econdidx == econdidx && m.captureblockidx == captureblockidx && m.fedid == fedid;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _electronicsMatch);
  return *it;
}

std::tuple<int,int,int> HGCalModuleLocator::getModuleLocation(int econdidx, int captureblockidx, int fedid) const
{
  auto _electronicsMatch = [econdidx, captureblockidx, fedid](HGCalModuleInfo m){ 
    return m.econdidx == econdidx && m.captureblockidx == captureblockidx && m.fedid == fedid;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _electronicsMatch);
  return std::make_tuple(it->plane, it->u, it->v);
}

std::tuple<int,int,int> HGCalModuleLocator::getModuleLocation(HGCalElectronicsId& id) const
{
  uint8_t econdidx = id.econdIdx();
  uint8_t captureblockidx = id.captureBlock();
  uint16_t fedid = id.fedId();
  
  auto _electronicsMatch = [econdidx, captureblockidx, fedid](HGCalModuleInfo m){ 
    return m.econdidx == econdidx && m.captureblockidx == captureblockidx && m.fedid == fedid;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _electronicsMatch);

  return std::make_tuple(it->plane, it->u, it->v);
}

int HGCalModuleLocator::getEcondIdx(int plane, int u, int v, int isSiPM) const
{
  auto _geometryMatch = [plane, u, v, isSiPM](HGCalModuleInfo m){ 
    return m.plane == plane && m.u == u && m.v == v && m.isSiPM == isSiPM;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _geometryMatch);

  return it->econdidx;
}

int HGCalModuleLocator::getCaptureBlockIdx(int plane, int u, int v, int isSiPM) const
{
  auto _geometryMatch = [plane, u, v, isSiPM](HGCalModuleInfo m){ 
    return m.plane == plane && m.u == u && m.v == v && m.isSiPM == isSiPM;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _geometryMatch);

  return it->captureblockidx;
}

int HGCalModuleLocator::getFedId(int plane, int u, int v, int isSiPM) const
{
  auto _geometryMatch = [plane, u, v, isSiPM](HGCalModuleInfo m){ 
    return m.plane == plane && m.u == u && m.v == v && m.isSiPM == isSiPM;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _geometryMatch);

  return it->fedid;
}

std::string HGCalModuleLocator::getDAQ(int plane, int u, int v, int isSiPM) const
{
  auto _geometryMatch = [plane, u, v, isSiPM](HGCalModuleInfo m){ 
    return m.plane == plane && m.u == u && m.v == v && m.isSiPM == isSiPM;
  };
  auto it = std::find_if(begin(mod2loc_.params_), end(mod2loc_.params_), _geometryMatch);

  return it->DAQ;
}
