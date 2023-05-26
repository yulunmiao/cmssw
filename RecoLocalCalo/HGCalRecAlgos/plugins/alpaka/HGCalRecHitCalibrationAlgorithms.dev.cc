// alpaka-related imports
#include <alpaka/alpaka.hpp>
#include "HeterogeneousCore/AlpakaInterface/interface/traits.h"
#include "HeterogeneousCore/AlpakaInterface/interface/workdivision.h"

// Calibration algorithms header
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"


namespace ALPAKA_ACCELERATOR_NAMESPACE {
  using namespace cms::alpakatools;
  
  class HGCalRecHitCalibrationKernel {
  public:
    template <typename TAcc, typename = std::enable_if_t<alpaka::isAccelerator<TAcc>>>
    ALPAKA_FN_ACC void operator()(TAcc const& acc,
                                  HGCalDeviceRecHitCollection::View view,
                                  int32_t size, double xvalue) const {
      // global index of the thread within the grid
      const int32_t thread = alpaka::getIdx<alpaka::Grid,alpaka::Threads>(acc)[0u];

      for (int32_t i : cms::alpakatools::elements_with_stride(acc, size)) {
        // view[i] = {i, i, 1., 2., 3., 4., 5., 6.};
      }
      // const portabletest::Matrix matrix{{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};

      // // set this only once in the whole kernel grid
      // if (thread == 0) view.r() = 1.;
  
      // // make a strided loop over the kernel grid, coveringup to "size" elements
      // for (int32_t i : cms::alpakatools::elements_with_stride(acc, size)) view[i] = {xvalue, 0., 0., i, matrix * i};
    }


  };


std::unique_ptr<HGCalDeviceRecHitCollection> HGCalRecHitCalibrationAlgorithms::calibrate(const std::unique_ptr<HGCalDeviceDigiCollection> &digis)
{
    auto recHits = std::make_unique<HGCalDeviceRecHitCollection>(); // = digisToRecHits(digits);
    // calibrate(recHits);

    return recHits;
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