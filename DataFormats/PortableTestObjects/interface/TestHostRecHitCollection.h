#ifndef DataFormats_PortableTestObjects_interface_TestHostRecHitCollection_h
#define DataFormats_PortableTestObjects_interface_TestHostRecHitCollection_h

#include "DataFormats/Portable/interface/PortableHostCollection.h"
#include "DataFormats/PortableTestObjects/interface/TestRecHitSoA.h"

namespace portabletest {

  // SoA with x, y, z, id fields in host memory
  using TestHostRecHitCollection = PortableHostCollection<TestRecHitSoA>;

}  // namespace portabletest

#endif  // DataFormats_PortableTestObjects_interface_TestHostRecHitCollection_h
