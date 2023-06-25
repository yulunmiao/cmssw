#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include <algorithm>

//
HGCalModuleInfo HGCalCondSerializableModuleInfo::getModule(int econdidx, int captureblockidx, int fedid, bool zside) const {
  auto _electronicsMatch = [econdidx, captureblockidx, fedid, zside](HGCalModuleInfo m){ 
    return m.econdidx == econdidx && m.captureblockidx == captureblockidx && m.fedid == fedid && m.zside==zside;
  };
  auto it = std::find_if(begin(params_), end(params_), _electronicsMatch);
  return *it;
}

//
HGCalModuleInfo HGCalCondSerializableModuleInfo::getModuleFromGeometry(int plane, int u, int v, bool isSiPM,bool zside) const {
  auto _geometryMatch = [plane, u, v, isSiPM, zside](HGCalModuleInfo m){
    return m.plane == plane && m.u == u && m.v == v && m.isSiPM == isSiPM && m.zside == zside;
  };
  auto it = std::find_if(begin(params_), end(params_), _geometryMatch);
  return *it;
}
  
//
std::tuple<int,int,int,bool> HGCalCondSerializableModuleInfo::getModuleLocation(int econdidx, int captureblockidx, int fedid,bool zside) const {
  auto it = getModule(econdidx,captureblockidx,fedid,zside);
  return std::make_tuple(it.plane, it.u, it.v, it.isSiPM);
}
  
//
std::tuple<int,int,int,bool> HGCalCondSerializableModuleInfo::getModuleLocation(HGCalElectronicsId& id) const {
  return getModuleLocation(id.econdIdx(), id.captureBlock(), id.fedId(), id.zSide());
}

//
std::tuple<int,int,int> HGCalCondSerializableModuleInfo::getModuleElectronicsIdentifiers(int plane, int u, int v, bool isSiPM, bool zside) const {
  auto it = getModuleFromGeometry(plane,u,v,isSiPM,zside);
  return std::make_tuple(it.fedid, it.captureblockidx, it.econdidx);
}
