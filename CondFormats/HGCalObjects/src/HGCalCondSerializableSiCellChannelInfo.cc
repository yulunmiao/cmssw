#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiCellChannelInfo.h"
#include <algorithm>
#include <iostream>

//
std::vector<HGCalSiCellChannelInfo> HGCalCondSerializableSiCellChannelInfo::getAllCellsInModule(bool isHD, uint8_t wafType) const {

  std::vector<HGCalSiCellChannelInfo> wafers;
  std::copy_if(params_.begin(), params_.end(), std::back_inserter(wafers), [&](HGCalSiCellChannelInfo v) {
     return (v.isHD == isHD) && (v.wafType == wafType);
  });
  
  return wafers;
}
