#ifndef DataFormats_PortableTestObjects_interface_alpaka_TestDeviceRecHitCollection_h
#define DataFormats_PortableTestObjects_interface_alpaka_TestDeviceRecHitCollection_h

#include "DataFormats/Portable/interface/alpaka/PortableCollection.h"
//#include "DataFormats/PortableTestObjects/interface/TestSoA.h"
#include "DataFormats/PortableTestObjects/interface/TestRecHitSoA.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  namespace portabletest {

    // make the names from the top-level portabletest namespace visible for unqualified lookup
    // inside the ALPAKA_ACCELERATOR_NAMESPACE::portabletest namespace
    using namespace ::portabletest;

    // SoA with x, y, z, id fields in device global memory
    using TestDeviceRecHitCollection = PortableCollection<TestRecHitSoA>;

  }  // namespace portabletest

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif  // DataFormats_PortableTestObjects_interface_alpaka_TestDeviceRecHitCollection_h
