#ifndef CondFormats_HGCalObjects_HGCalCondSerializableSiPMTileInfo_h
#define CondFormats_HGCalObjects_HGCalCondSerializableSiPMTileInfo_h

#include <vector>
#include <map>
#include <string>

#include "CondFormats/Serialization/interface/Serializable.h"

struct HGCalSiPMTileInfo {
  int sipmcell,plane,iring,iphi,trigch,trigsum,modiring;
  std::string t;
  COND_SERIALIZABLE;
};

/**
   @short cond. serializable class holding information on a sipm-on-tile cell
*/
class HGCalCondSerializableSiPMTileInfo {
  
 public:
    
  HGCalCondSerializableSiPMTileInfo() {}
  virtual ~HGCalCondSerializableSiPMTileInfo() {}
  inline HGCalCondSerializableSiPMTileInfo& addParameter(HGCalSiPMTileInfo &i) {
    params_.push_back(i);
    return *this;
  }
  std::vector<HGCalSiPMTileInfo> params_;

  COND_SERIALIZABLE;
};

#endif
