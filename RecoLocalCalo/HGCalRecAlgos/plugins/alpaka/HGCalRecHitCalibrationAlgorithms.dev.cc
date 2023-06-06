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

      // dummy digis -> rechits conversion (to be replaced by the actual formula)
      for (auto index : elements_with_stride(acc, digis.metadata().size())) {
        recHits[index].detid() = static_cast<uint32_t>(digis[index].electronicsId());
        recHits[index].energy() = static_cast<double>(digis[index].raw());
        recHits[index].time() = static_cast<double>(digis[index].cm());
        recHits[index].flags() = digis[index].flags();
      }
    }
  };

  class HGCalRecHitCalibrationKernel_pedestalCorrection {
  public:
    template <typename TAcc, typename T>
    ALPAKA_FN_ACC void operator()(TAcc const& acc, T recHits, float pedestalValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= pedestalValue;
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

  // applySomeCalibration(recHits);
  // applySomeOtherCalibration(recHits);

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