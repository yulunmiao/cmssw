#ifndef CondFormats_HGCalObjects_HGCalCondSerializablePedestals_h
#define CondFormats_HGCalObjects_HGCalCondSerializablePedestals_h

#include <map>
#include <cstdint>

#include "CondFormats/Serialization/interface/Serializable.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

/**
   @short representation of a si-cell channel information read from a txt file or db
*/
struct HGCalPedestals {
  uint16_t pedestal, cm_slope, cm_offset, kappa_bxm1;
  COND_SERIALIZABLE;
};

/**
   @holder for the si cell channel mapping
 */
class HGCalCondSerializablePedestals {
  
public:
  
  HGCalCondSerializablePedestals() {}
  virtual ~HGCalCondSerializablePedestals() {}
  inline HGCalCondSerializablePedestals& addParameter(uint32_t id,HGCalPedestals &p) {
    if (params_.count(id) != 0)
      throw cms::Exception("HGCalCondSerializablePedestals") << "Already existing parameter with id='" << id << "'.";
    params_[id] = p;
    return *this;
  }
  inline const HGCalPedestals& parameters(uint32_t id) {
    if (params_.count(id) != 0)
      throw cms::Exception("HGCalCondSerializablePedestals") << "Failed to retrieve parameter with id='" << id << "'.";
    return params_[id];
  }
  
  std::map<uint32_t,HGCalPedestals> params_;
  
  COND_SERIALIZABLE;
};

#endif
