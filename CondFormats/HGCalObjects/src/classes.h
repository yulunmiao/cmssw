#include "CondFormats/HGCalObjects/src/headers.h"

namespace CondFormats_HGCalObjects {

  std::vector<int> v_i;
  std::pair<std::string, std::vector<int> > p_s_v_i;
  std::map<std::string, std::vector<int> > m_s_v_i;
  HGCalCondSerializableGenericConfig h_csgc();
  
  HGCalSiCellChannelInfo hscci;
  std::vector<HGCalSiCellChannelInfo> v_hscci;
  HGCalCondSerializableSiCellChannelInfo h_cscci();

  HGCalSiPMTileInfo hsti;
  std::vector<HGCalSiPMTileInfo> v_hsti;
  HGCalCondSerializableSiPMTileInfo h_csti();

  HGCalModuleInfo hmi;
  std::vector<HGCalModuleInfo> v_hmi;
  HGCalCondSerializableModuleInfo h_csmi();

}  // namespace CondFormats_HGCalObjects
