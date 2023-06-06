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

#include "DataFormats/HGCalDigi/interface/HGCalHostDigiCollection.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace hgcaldigi;
  using namespace hgcalrechit;

  class HGCalRecHitCalibrationAlgorithms {
  public:
    std::unique_ptr<HGCalDeviceRecHitCollection> calibrate(const HGCalHostDigiCollection &digis);

    // if converting host digis to device rechits turns out too slow, we should copy host digis to device digis and then
    // convert to device rechits on device
    // std::unique_ptr<HGCalDeviceRecHitCollection> calibrate(const std::unique_ptr<HGCalDeviceDigiCollection> &digis);

  private:
    void print(const std::unique_ptr<HGCalDeviceRecHitCollection> &recHits, int max=-1);
    void print(const HGCalHostDigiCollection &digis, int max=-1);
  };

} // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif // __HGCalRecHitCalibrationAlgorithms_H__
