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
    
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDigiDeviceCollection::View digis, HGCalRecHitDeviceCollection::View recHits) const {
      auto ToA_to_time = [&](uint32_t ToA) { return float(ToA); };
      auto ADC_to_energy = [&](uint32_t ADC) { return float(ADC); };
      // auto ToT_to_energy = [&](uint16_t ToT){return float(ToT);};

      // dummy digis -> rechits conversion (to be replaced by the actual formula)
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        recHits[index].detid() = static_cast<uint32_t>(digis[index].electronicsId());
        recHits[index].energy() = ADC_to_energy(digis[index].adc());
        recHits[index].time() = ToA_to_time(digis[index].toa());
        recHits[index].flags() = digis[index].flags();
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_pedestalCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDigiDeviceCollection::View digis, float pedestalValue) const {
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        if ((digis[index].flags() >> kPedestalCorrection) & 1){
          //calibParams[digis[index].electronicsId()];
          digis[index].adc() -= pedestalValue;
        }
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_commonModeCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDigiDeviceCollection::View digis, float commonModeValue) const {
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        digis[index].adc() -= commonModeValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_ADCmCorrection {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalDigiDeviceCollection::View digis, float ADCmValue) const {
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        digis[index].adc() -= ADCmValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_printRecHits {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, HGCalRecHitDeviceCollection::ConstView view, int size) const {
      for (int i = 0; i < size; ++i) {
        auto const& rechit = view[i];
        printf("%d\t%d\t%f\t%f\t%d\n", i, rechit.detid(), rechit.energy(), rechit.time(), rechit.flags());
      }
      printf("\n");
    }
  };

  void HGCalRecHitCalibrationAlgorithms::loadCalibParams(CalibParams& newCalibParams) {
    std::cout << "\nINFO -- HGCalRecHitCalibrationAlgorithms::loadCalibParams: " << newCalibParams.size() << " elements" << std::endl;
    calibParams = newCalibParams;
  }

  std::unique_ptr<HGCalRecHitDeviceCollection> HGCalRecHitCalibrationAlgorithms::calibrate(Queue& queue, HGCalDigiHostCollection & digis) {
    std::cout << "\n\nINFO -- Start of calibrate\n\n" << std::endl;

    std::cout<<"N blocks: "<<n_blocks<<"\tN threads: "<<n_threads<<std::endl;
    auto grid = make_workdiv<Acc1D>(n_blocks, n_threads);

    bool verbose = true;
    int n_hits_to_print = 10;
    
    if(verbose) std::cout << "Input digis: " << std::endl;
    if(verbose) print(digis, n_hits_to_print);
    
    if(verbose) std::cout << "\n\nINFO -- copying the digis to the device\n\n" << std::endl;
    HGCalDigiDeviceCollection device_digis(digis.view().metadata().size(), queue);
    alpaka::memcpy(queue, device_digis.buffer(), digis.const_buffer());

    float pedestalValue = n_hits_to_print; // dummy value
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_pedestalCorrection{}, digis.view(), pedestalValue);
    if(verbose) std::cout << "Digis after pedestal calibration: " << std::endl;
    if(verbose) print(digis, n_hits_to_print);

    float commonModeValue = n_hits_to_print; // dummy value
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_commonModeCorrection{}, digis.view(), commonModeValue);
    if(verbose) std::cout << "Digis after CM calibration: " << std::endl;
    if(verbose) print(digis, n_hits_to_print);

    float ADCmValue = n_hits_to_print; // dummy value
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_ADCmCorrection{}, digis.view(), ADCmValue);
    if(verbose) std::cout << "Digis after ADCm calibration: " << std::endl;
    if(verbose) print(digis, n_hits_to_print);

    if(verbose) std::cout << "\n\nINFO -- allocating rechits buffer" << std::endl;
    auto recHits = std::make_unique<HGCalRecHitDeviceCollection>(digis.view().metadata().size(), queue);
    
    if(verbose) std::cout << "\n\nINFO -- converting digis to rechits" << std::endl;
    alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_digisToRecHits{}, device_digis.view(), recHits->view());
    
    if(verbose) std::cout << "RecHits after calibration: " << std::endl;
    if(verbose) print(queue, *recHits, n_hits_to_print);

    return recHits;
  }

  void HGCalRecHitCalibrationAlgorithms::print(HGCalDigiHostCollection const& digis, int max) const {
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

  void HGCalRecHitCalibrationAlgorithms::print(Queue& queue, HGCalRecHitDeviceCollection const& recHits, int max) const {
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

//   void HGCalRecHitCalibrationAlgorithms::fill(Queue& queue, HGCalRecHitDeviceCollection& collection, double xvalue) const {
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
