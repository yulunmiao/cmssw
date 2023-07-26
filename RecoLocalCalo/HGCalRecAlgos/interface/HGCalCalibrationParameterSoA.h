#ifndef RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterSoA_h
#define RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterSoA_h

#include <Eigen/Core>
#include <Eigen/Dense>

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterProvider.h"

namespace hgcalrechit {

  // Generate structure of arrays (SoA) layout with RecHit dataformat
  GENERATE_SOA_LAYOUT(HGCalCalibrationParameterSoALayout,
                      SOA_SCALAR(HGCalCalibrationParameterProviderConfig, config),
                      SOA_COLUMN(float, pedestal),
                      SOA_COLUMN(float, CM_slope),
                      SOA_COLUMN(float, CM_offset),
                      SOA_COLUMN(float, BXm1_kappa)
  )
  using HGCalCalibParamSoA = HGCalCalibrationParameterSoALayout<>;
  
}  // namespace hgcalrechit

#endif  // RecoLocalCalo_HGCalRecAlgos_interface_HGCalCalibrationParameterSoA_h
