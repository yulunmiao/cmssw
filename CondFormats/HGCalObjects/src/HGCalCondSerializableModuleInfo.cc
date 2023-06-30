#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include <algorithm>

//
HGCalModuleInfo HGCalCondSerializableModuleInfo::getModule(int econdidx, int captureblock, int slink, bool zside) const {
  auto _electronicsMatch = [econdidx, captureblock, slink, zside](HGCalModuleInfo m){ 
    return m.econdidx == econdidx && m.captureblock == captureblock && m.slink == slink && m.zside==zside;
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
std::tuple<int,int,int,bool> HGCalCondSerializableModuleInfo::getModuleLocation(int econdidx, int captureblock, int slink,bool zside) const {
  auto it = getModule(econdidx,captureblock,slink,zside);
  return std::make_tuple(it.plane, it.u, it.v, it.isSiPM);
}
  
//
std::tuple<int,int,int,bool> HGCalCondSerializableModuleInfo::getModuleLocation(HGCalElectronicsId& id) const {
  return getModuleLocation(id.econdIdx(), id.captureBlock(), id.fedId(), id.zSide());
}

//
std::tuple<int,int,int> HGCalCondSerializableModuleInfo::getModuleElectronicsIdentifiers(int plane, int u, int v, bool isSiPM, bool zside) const {
  auto it = getModuleFromGeometry(plane,u,v,isSiPM,zside);
  return std::make_tuple(it.slink, it.captureblock, it.econdidx);
}

//
std::map<HGCalCondSerializableModuleInfo::ModuleInfoKey_t,HGCalCondSerializableModuleInfo::ModuleInfoKey_t> HGCalCondSerializableModuleInfo::getAsSimplifiedModuleLocatorMap(bool elecidAsKey) const {

  std::map<ModuleInfoKey_t,ModuleInfoKey_t> module_keys;
  for(auto m : params_) {
    ModuleInfoKey_t logiKey(m.zside,m.slink,m.captureblock,m.econdidx);
    ModuleInfoKey_t geomKey(m.zside,m.plane,m.u,m.v);
    module_keys[elecidAsKey ? logiKey : geomKey] = elecidAsKey ? geomKey : logiKey;
  }
  
  return module_keys;
}

//
std::tuple<uint16_t,uint16_t,uint16_t,uint16_t> HGCalCondSerializableModuleInfo::getMaxValuesForDenseIndex() const {

  uint16_t maxslink(0),maxcaptureblock(0),maxecondidx(0),maxerx(0);
  for(auto m : params_) {
    maxslink=std::max(m.slink,maxslink);
    maxcaptureblock=std::max(m.captureblock,maxcaptureblock);
    maxecondidx=std::max(m.econdidx,maxecondidx);
    uint16_t nerx=6*(1+m.isHD);
    maxerx=std::max(nerx,maxerx);
  }

  return std::tuple<uint16_t,uint16_t,uint16_t,uint16_t>(maxslink+1,maxcaptureblock+1,maxecondidx+1,maxerx);
}

//
HGCalCondSerializableModuleInfo::ERxBitPatternMap HGCalCondSerializableModuleInfo::getERxBitPattern() const {

  std::tuple<uint16_t,uint16_t,uint16_t,uint16_t> maxValsForDenseIdx=getMaxValuesForDenseIndex();
  uint16_t maxCB=std::get<1>(maxValsForDenseIdx);
  uint16_t maxEcon=std::get<2>(maxValsForDenseIdx);
  
  HGCalCondSerializableModuleInfo::ERxBitPatternMap erxbit;
  for(auto m : params_) {
    uint32_t rtn = erxBitPatternMapDenseIndex(m.slink,m.captureblock,m.econdidx,maxCB,maxEcon);
    uint8_t nerx=6*(1+m.isHD);
    erxbit[rtn]=(1<<nerx)-1;
  }
  return erxbit;
}
