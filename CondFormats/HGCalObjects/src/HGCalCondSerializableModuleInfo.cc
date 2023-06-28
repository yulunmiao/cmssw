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

//
std::map<HGCalCondSerializableModuleInfo::ModuleInfoKey_t,HGCalCondSerializableModuleInfo::ModuleInfoKey_t> HGCalCondSerializableModuleInfo::getAsSimplifiedModuleLocatorMap(bool elecidAsKey) const {

  std::map<ModuleInfoKey_t,ModuleInfoKey_t> module_keys;
  for(auto m : params_) {
    ModuleInfoKey_t logiKey(m.zside,m.fedid,m.captureblock,m.econdidx);
    ModuleInfoKey_t geomKey(m.zside,m.plane,m.u,m.v);
    module_keys[elecidAsKey ? logiKey : geomKey] = elecidAsKey ? geomKey : logiKey;
  }
  
  return module_keys;
}

//
std::tuple<uint16_t,uint16_t,uint16_t,uint16_t> HGCalCondSerializableModuleInfo::getMaxValuesForDenseIndex() const {

  uint16_t maxfedid(0),maxcaptureblock(0),maxecondidx(0),maxerx(0);
  for(auto m : params_) {
    maxfedid=std::max(m.fedid,maxfedid);
    maxcaptureblock=std::max(m.captureblock,maxcaptureblock);
    maxecondidx=std::max(m.econdidx,maxecondidx);
    uint16_t nerx=6*(1+m.isHD);
    maxerx=std::max(nerx,maxerx);
  }

  return std::tuple<uint16_t,uint16_t,uint16_t,uint16_t>(maxfedid,maxcaptureblock,maxecondidx,maxerx);
}

//
HGCalCondSerializableModuleInfo::ERxBitPatternMap HGCalCondSerializableModuleInfo::getERxBitPattern() const {
  HGCalCondSerializableModuleInfo::ERxBitPatternMap erxbit;
  for(auto m : params_) {
    std::tuple<uint16_t,uint16_t,uint16_t> key(m.fedid,m.captureblock,m.econdidx);
    uint8_t nerx=6*(1+m.isHD);
    erxbit[key]=(1<<nerx)-1;
  }
  return erxbit;
}
