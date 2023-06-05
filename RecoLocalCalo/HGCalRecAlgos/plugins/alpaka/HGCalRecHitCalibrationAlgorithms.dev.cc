// alpaka-related imports
#include <alpaka/alpaka.hpp>
#include "HeterogeneousCore/AlpakaInterface/interface/traits.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

// Calibration algorithms header
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;
  using namespace std;

  typedef std::unique_ptr<HGCalDeviceDigiCollection> Digis;
  typedef std::unique_ptr<HGCalDeviceRecHitCollection> RecHits;

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
    ALPAKA_FN_ACC void operator()(TAcc const& acc, T &recHits, float pedestalValue) const {
      for (auto index : elements_with_stride(acc, recHits.metadata().size())) {
        recHits[index].energy() -= pedestalValue;
      }
    }
  };

std::unique_ptr<HGCalDeviceRecHitCollection> HGCalRecHitCalibrationAlgorithms::calibrate(const std::unique_ptr<HGCalDeviceDigiCollection> &digis)
{
  cout<<"\n\nINFO -- Start of calibrate\n\n"<<endl;

  auto queue = Queue(devices<Platform>()[0]); // should pick the right device somehow...
  auto grid = make_workdiv<Acc1D>(4, 4);

  auto recHits = std::make_unique<HGCalDeviceRecHitCollection>();

  cout<<"INFO -- converting digis to rechits"<<endl;

  alpaka::exec<Acc1D>(queue, 
                      grid, 
                      HGCalRecHitCalibrationKernel_digisToRecHits{}, 
                      digis->view(),
                      recHits->view()
                      // recHits->view()
  );

  std::cout << "Input digis: " << std::endl;

  for(int i=0; i<digis->view().metadata().size(); i++){
    std::cout<<i;
    std::cout<<"\t"<<digis->view()[i].electronicsId();
    std::cout<<"\t"<<digis->view()[i].raw();
    std::cout<<"\t"<<digis->view()[i].cm();
    std::cout<<"\t"<<digis->view()[i].flags();
  }


  std::cout << "RecHits: " << std::endl;

  for(int i=0; i<recHits->view().metadata().size(); i++){
    std::cout<<i;
    std::cout<<"\t"<<recHits->view()[i].detid();
    std::cout<<"\t"<<recHits->view()[i].energy();
    std::cout<<"\t"<<recHits->view()[i].time();
    std::cout<<"\t"<<recHits->view()[i].flags();
  }

  
  // float pedestalValue = 10;
  // alpaka::exec<Acc1D>(queue, grid, HGCalRecHitCalibrationKernel_pedestalCorrection{}, recHits->view(), pedestalValue);
  
  // applySomeCalibration(recHits);
  // applySomeOtherCalibration(recHits);

    return recHits;
}

} // namespace ALPAKA_ACCELERATOR_NAMESPACE


//
// Some potentially useful code snippets:
//

// class HGCalRecHitCalibrationKernel {
// public:
//   template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
//   ALPAKA_FN_ACC void operator()(TAcc const& acc,
//                                 HGCalDeviceRecHitCollection::View view,
//                                 int32_t size, double xvalue) const {
//     // global index of the thread within the grid
//     const int32_t thread = alpaka::getIdx<alpaka::Grid,alpaka::Threads>(acc)[0u];

//     for (int32_t i : elements_with_stride(acc, size)) {
//       // view[i] = {i, i, 1., 2., 3., 4., 5., 6.};
//     }
//     // const portabletest::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};

//     // // set this only once in the whole kernel grid
//     // if (thread == 0) view.r() = 1.;

//     // // make a strided loop over the kernel grid, coveringup to "size" elements
//     // for (int32_t i : elements_with_stride(acc, size)) view[i] = {xvalue, 0., 0., i, matrix * i};
//   }
// };


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