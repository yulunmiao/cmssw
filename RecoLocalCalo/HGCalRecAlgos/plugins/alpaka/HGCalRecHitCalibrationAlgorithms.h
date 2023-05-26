#ifndef __HGCalRecHitCalibrationAlgorithms_H__
#define __HGCalRecHitCalibrationAlgorithms_H__

// alpaka-related imports
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

// HGCal digis data formats
#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"

// Host and devide HGCal RecHit data formats
#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDeviceDigiCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalDeviceRecHitCollection.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace hgcaldigi;
  using namespace hgcalrechit;

  class HGCalRecHitCalibrationAlgorithms {
  public:
    std::unique_ptr<HGCalDeviceRecHitCollection> calibrate(const std::unique_ptr<HGCalDeviceDigiCollection> &digis);
  };

} // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif // __HGCalRecHitCalibrationAlgorithms_H__
