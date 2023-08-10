#ifndef RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterHostCollection_h
#define RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterHostCollection_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterSoA.h"

namespace hgcalrechit {

  // SoA with pedestal, CM_slope, CM_offset, BXm1_kappa in host memory
  using HGCalCalibParamHostCollection = PortableHostCollection<HGCalCalibParamSoA>;

  // SoA with gain in host memory
  using HGCalConfigParamHostCollection = PortableHostCollection<HGCalConfigParamSoA>;

}  // namespace hgcalrechit

#endif  // RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterHostCollection_h
