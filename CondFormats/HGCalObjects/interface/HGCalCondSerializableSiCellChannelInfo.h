#ifndef CondFormats_HGCalObjects_HGCalCondSerializableSiCellChannelInfo_h
#define CondFormats_HGCalObjects_HGCalCondSerializableSiCellChannelInfo_h

#include <vector>
#include <cstdint>

#include "CondFormats/Serialization/interface/Serializable.h"
/**
   @short representation of a si-cell channel information read from a txt file or db
*/
struct HGCalSiCellChannelInfo {
  bool isHD,iscalib;
  uint8_t wafType, chip, half;
  uint16_t seq,rocpin;
  int sicell,triglink,trigcell,iu,iv,t;
  float trace;
  COND_SERIALIZABLE;
};

/**
   @holder for the si cell channel mappeing
 */
class HGCalCondSerializableSiCellChannelInfo {
  
public:
  
  HGCalCondSerializableSiCellChannelInfo() {}
  virtual ~HGCalCondSerializableSiCellChannelInfo() {}
  inline HGCalCondSerializableSiCellChannelInfo& addParameter(HGCalSiCellChannelInfo &i) {
    params_.push_back(i);
    return *this;
  }
  std::vector<HGCalSiCellChannelInfo> getAllCellsInModule(bool isHD,uint8_t wafType) const;
  std::vector<HGCalSiCellChannelInfo> params_;
  
  COND_SERIALIZABLE;
};

#endif
