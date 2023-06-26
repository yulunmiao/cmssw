#ifndef __HGCalRecHitCalibrationAlgorithms_H__
#define __HGCalRecHitCalibrationAlgorithms_H__

// alpaka-related imports
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

// HGCal digis data formats
#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"

// Host and devide HGCal RecHit data formats
#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDigiDeviceCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalRecHitDeviceCollection.h"

#include "DataFormats/HGCalDigi/interface/HGCalDigiHostCollection.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace hgcaldigi;
  using namespace hgcalrechit;

  class HGCalRecHitCalibrationAlgorithms {
  public:
    HGCalRecHitCalibrationAlgorithms(int n_blocks_, int n_threads_) : n_blocks(n_blocks_), n_threads(n_threads_) {}

    std::unique_ptr<HGCalRecHitDeviceCollection> calibrate(Queue& queue, HGCalDigiHostCollection const& digis);

    // if converting host digis to device rechits turns out too slow, we should copy host digis to device digis and then
    // convert to device rechits on device
    // std::unique_ptr<HGCalRecHitDeviceCollection> calibrate(Queue& queue, const std::unique_ptr<HGCalDigiDeviceCollection> &digis);

    typedef std::map<HGCalElectronicsId,float> CalibParams; // ID -> pedestal
    void loadCalibParams(CalibParams& newCalibParams);

  private:
    void print(Queue& queue, HGCalRecHitDeviceCollection const& recHits, int max = -1) const;
    void print(HGCalDigiHostCollection const& digis, int max = -1) const;

    int n_blocks;
    int n_threads;
    CalibParams calibParams; // ID -> pedestal

  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // __HGCalRecHitCalibrationAlgorithms_H__
