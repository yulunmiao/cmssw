#ifndef CondFormats_HGCalObjects_HGCalCondSerializableModuleInfo_h
#define CondFormats_HGCalObjects_HGCalCondSerializableModuleInfo_h

#include <vector>
#include <cstdint>
#include <tuple>

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "CondFormats/Serialization/interface/Serializable.h"

/**
   @short representation of a si-cell channel information read from a txt file or db
*/
struct HGCalModuleInfo {
  bool zside,isSiPM,isHD;
  int plane, u, v, modtype;
  int econdidx, captureblock, slink, captureblockidx, fedid;
  COND_SERIALIZABLE;
};

/**
   @holder for the si cell channel mappeing
 */
class HGCalCondSerializableModuleInfo {
  
public:
  
  HGCalCondSerializableModuleInfo() {}
  virtual ~HGCalCondSerializableModuleInfo() {}

  /**
     @short add module to collection
   */
  inline HGCalCondSerializableModuleInfo& addParameter(HGCalModuleInfo &i) {
    params_.push_back(i);
    return *this;
  }

  /**
     @short retrieve module info from electronics id information (ECON-D idx, Capture Block idx, FED ID)
  */
  HGCalModuleInfo getModule(int ,int ,int ,bool) const;

  /**
     @short retrieve module info from geometry
  */
  HGCalModuleInfo getModuleFromGeometry(int ,int ,int ,bool ,bool ) const;
  
  /**
     @short Module location from electronics id information (plane,u,v,isSiPM)
  */
  std::tuple<int,int,int,bool> getModuleLocation(int ,int ,int ,bool) const;
  
  /**
     @short Module location from ElectronicsId (plane,u,v,isSiPM)
  */
  std::tuple<int,int,int,bool> getModuleLocation(HGCalElectronicsId& ) const;

  /*
    @short module electronics identifier (ECON-D idx, Capture Block idx, FED ID)
  */
  std::tuple<int,int,int> getModuleElectronicsIdentifiers(int ,int ,int ,bool , bool ) const;

  //parameters to serialize
  std::vector<HGCalModuleInfo> params_;
  
  COND_SERIALIZABLE;
};

#endif
