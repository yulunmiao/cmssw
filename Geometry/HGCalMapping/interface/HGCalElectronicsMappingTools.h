#ifndef Geometry_HGCalMapping_interface_HGCalElectronicsMappingTools
#define Geometry_HGCalMapping_interface_HGCalElectronicsMappingTools

#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiCellChannelInfo.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"

namespace hgcal {

  /**
     @short method returns a DetId to ElectronicsId map or vice-versa based on the info available for Si cells
   */
  std::map<uint32_t,uint32_t> mapSiGeoToElectronics(const HGCalCondSerializableModuleInfo &, const HGCalCondSerializableSiCellChannelInfo &,bool geo2ele);

  /**
     @short formula to get ECOND e-Rx for a given ROC chip/half
   */
  uint16_t getEcondErxFor(uint16_t chip,uint16_t half) { return chip*2+half; }
};

#endif
