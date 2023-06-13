// alpaka-related imports
#include <alpaka/alpaka.hpp>
#include "HeterogeneousCore/AlpakaInterface/interface/traits.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

// Calibration algorithms header
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;

  enum HGCalCalibrationFlag {
      kPedestalCorrection=0,
      kCMCorrection,
      kADCmCorrection,
  };

  class HGCalRecHitCalibrationKernel_digisToRecHits {
  public:
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDeviceDigiCollection::ConstView digis, HGCalDeviceRecHitCollection::View recHits) const {
      auto ToA_to_time = [&](uint16_t ToA) { return float(ToA); };
      auto ADC_to_energy = [&](uint16_t ADC) { return float(ADC); };
      // auto ToT_to_energy = [&](uint16_t ToT){return float(ToT);};

      // dummy digis -> rechits conversion (to be replaced by the actual formula)
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        recHits[index].detid() = static_cast<uint32_t>(digis[index].electronicsId());
        recHits[index].energy() = ADC_to_energy(digis[index].raw());
        recHits[index].time() = ToA_to_time(digis[index].cm());
        recHits[index].flags() = digis[index].flags();
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_pedestalCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDeviceRecHitCollection::View recHits, float pedestalValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        if ((recHits[index].flags() >> kPedestalCorrection) & 1){
          recHits[index].energy() -= pedestalValue;
        }
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_commonModeCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDeviceRecHitCollection::View recHits, float commonModeValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= commonModeValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_ADCmCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDeviceRecHitCollection::View recHits, float ADCmValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= ADCmValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_printRecHits {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDeviceRecHitCollection::ConstView view, int size) const {
      for (int i = 0; i < size; ++i) {
        auto const& rechit = view[i];
        printf("%d\t%d\t%f\t%f\t%d\n", i, rechit.detid(), rechit.energy(), rechit.time(), rechit.flags());
      }
      printf("\n");
    }
  };

  std::unique_ptr<HGCalDeviceRecHitCollection> HGCalRecHitCalibrationAlgorithms::calibrate(Queue& queue, HGCalHostDigiCollection const& digis) {
    std::cout << "\n\nINFO -- Start of calibrate\n\n" << std::endl;

    auto grid = make_workdiv<Acc1D>(4, 4);

    std::cout << "Input digis: " << std::endl;
    print(digis, 10);

    std::cout << "\n\nINFO -- allocating rechits buffer" << std::endl;
    auto recHits = std::make_unique<HGCalDeviceRecHitCollection>(digis.view().metadata().size(), queue);
    print(queue, *recHits, 10);

    std::cout << "\n\nINFO -- copying the digis to the device\n\n" << std::endl;
    HGCalDeviceDigiCollection device_digis(digis.view().metadata().size(), queue);
    alpaka::memcpy(queue, device_digis.buffer(), digis.const_buffer());

    std::cout << "\n\nINFO -- converting digis to rechits" << std::endl;
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_digisToRecHits{}, device_digis.const_view(), recHits->view());

    std::cout << "RecHits before calibration: " << std::endl;
    print(queue, *recHits, 10);

    float pedestalValue = 10;
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_pedestalCorrection{}, recHits->view(), pedestalValue);

    std::cout << "RecHits after pedestal calibration: " << std::endl;
    print(queue, *recHits, 10);

    float commonModeValue = 10;
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_commonModeCorrection{}, recHits->view(), commonModeValue);

    std::cout << "RecHits after CM calibration: " << std::endl;
    print(queue, *recHits, 10);

    float ADCmValue = 10;
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_ADCmCorrection{}, recHits->view(), ADCmValue);

    std::cout << "RecHits after ADCm calibration: " << std::endl;
    print(queue, *recHits, 10);

    return recHits;
  }

  void HGCalRecHitCalibrationAlgorithms::print(HGCalHostDigiCollection const& digis, int max) const {
    int max_ = max > 0 ? max : digis.view().metadata().size();
    for (int i = 0; i < max_; i++) {
      std::cout << i;
      std::cout << "\t" << digis.view()[i].electronicsId();
      std::cout << "\t" << digis.view()[i].raw();
      std::cout << "\t" << digis.view()[i].cm();
      std::cout << "\t" << digis.view()[i].flags();
      std::cout << std::endl;
    }
  }

  void HGCalRecHitCalibrationAlgorithms::print(Queue& queue, HGCalDeviceRecHitCollection const& recHits, int max) const {
    auto grid = make_workdiv<Acc1D>(1, 1);
    auto size = max > 0 ? max : recHits.view().metadata().size();
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_printRecHits{}, recHits.view(), size);
    // ensure that the print operations are complete before returning
    alpaka::wait(queue);
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

//
// Some potentially useful code snippets:
//

//   void HGCalRecHitCalibrationAlgorithms::fill(Queue& queue, HGCalDeviceRecHitCollection& collection, double xvalue) const {
//     // use 64 items per group (this value is arbitrary, but it's a reasonable starting point)
//     uint32_t items = 64;

//     // use as many groups as needed to cover the whole problem
//     uint32_t groups = divide_up_by(collection->metadata().size(), items);

//     // map items to
//     // -threadswithasingleelementperthreadonaGPUbackend
//     // -elementswithinasinglethreadonaCPUbackend
//     auto workDiv = make_workdiv<Acc1D>(groups, items);

//     alpaka::exec<Acc1D>(queue, workDiv, HGCalRecHitCalibrationKernel{}, collection.view(), collection->metadata().size(), xvalue);
//   }
