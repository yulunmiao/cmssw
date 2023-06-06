// alpaka-related imports
#include <alpaka/alpaka.hpp>
#include "HeterogeneousCore/AlpakaInterface/interface/traits.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

// Calibration algorithms header
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;
  using namespace std;

  class HGCalRecHitCalibrationKernel_digisToRecHits {
  public:
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, const T &digis,
                                  HGCalDeviceRecHitCollection::View recHits) const {
      

      auto ToA_to_time = [&](uint16_t ToA){return float(ToA);};
      auto ADC_to_energy = [&](uint16_t ADC){return float(ADC);};
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
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, T recHits, float pedestalValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= pedestalValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_commonModeCorrection {
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, T recHits, float commonModeValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= commonModeValue;
      }
    }
  };

  struct HGCalRecHitCalibrationKernel_ADCmCorrection {
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, T recHits, float ADCmValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= ADCmValue;
      }
    }
  };

unique_ptr<HGCalDeviceRecHitCollection> HGCalRecHitCalibrationAlgorithms::calibrate(const HGCalHostDigiCollection &digis)
{
  cout<<"\n\nINFO -- Start of calibrate\n\n"<<endl;

  auto queue = Queue(devices<Platform>()[0]); // should pick the right device somehow...
  auto grid = make_workdiv<Acc1D>(4, 4);

  cout << "Input digis: " << endl;
  print(digis, 10);
  
  cout<<"\n\nINFO -- converting digis to rechits"<<endl;
  auto recHits = make_unique<HGCalDeviceRecHitCollection>(digis.view().metadata().size(), queue);

  alpaka::exec<Acc1D>(queue, 
                      grid, 
                      HGCalRecHitCalibrationKernel_digisToRecHits{}, 
                      digis.view(),
                      recHits->view()
  );

  cout << "RecHits before calibration: " << endl;
  print(recHits, 10);

  float pedestalValue = 10;
  alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_pedestalCorrection{}, recHits->view(), pedestalValue);
  cout << "RecHits after pedestal calibration: " << endl;
  print(recHits, 10);

  float commonModeValue = 10;
  alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_commonModeCorrection{}, recHits->view(), commonModeValue);
  cout << "RecHits after CM calibration: " << endl;
  print(recHits, 10);

  float ADCmValue = 10;
  alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_ADCmCorrection{}, recHits->view(), ADCmValue);
  cout << "RecHits after ADCm calibration: " << endl;
  print(recHits, 10);

  return recHits;
}

void HGCalRecHitCalibrationAlgorithms::print(const HGCalHostDigiCollection &digis, int max){
  int max_ = max > 0 ? max : digis.view().metadata().size();
  for(int i=0; i<max_; i++){
    cout<<i;
    cout<<"\t"<<digis.view()[i].electronicsId();
    cout<<"\t"<<digis.view()[i].raw();
    cout<<"\t"<<digis.view()[i].cm();
    cout<<"\t"<<digis.view()[i].flags();
    cout<<endl;
  }
}

void HGCalRecHitCalibrationAlgorithms::print(const unique_ptr<HGCalDeviceRecHitCollection> &recHits, int max){
  int max_ = max > 0 ? max : recHits->view().metadata().size();
  for(int i=0; i<max_; i++){
    cout<<i;
    cout<<"\t"<<recHits->view()[i].detid();
    cout<<"\t"<<recHits->view()[i].energy();
    cout<<"\t"<<recHits->view()[i].time();
    cout<<"\t"<<recHits->view()[i].flags();
    cout<<endl;
  }
}

} // namespace ALPAKA_ACCELERATOR_NAMESPACE


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