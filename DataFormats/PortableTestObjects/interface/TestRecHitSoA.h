#ifndef DataFormats_PortableTestObjects_interface_TestRecHitSoA_h
#define DataFormats_PortableTestObjects_interface_TestRecHitSoA_h

#include <Eigen/Core>
#include <Eigen/Dense>

#include "DataFormats/SoATemplate/interface/SoACommon.h"
#include "DataFormats/SoATemplate/interface/SoALayout.h"
#include "DataFormats/SoATemplate/interface/SoAView.h"

namespace portabletest {

  //using Matrix = Eigen::Matrix<double, 3, 6>;
  //// SoA layout with x, y, z, id fields
  //GENERATE_SOA_LAYOUT(TestRecHitSoALayout,
  //                    // columns: one value per element
  //                    SOA_COLUMN(double, x),
  //                    SOA_COLUMN(double, y),
  //                    SOA_COLUMN(double, z),
  //                    SOA_COLUMN(int32_t, id),
  //                    // scalars: one value for the whole structure
  //                    SOA_SCALAR(double, r),
  //                    // Eigen columns
  //                    // the typedef is needed because commas confuse macros
  //                    SOA_EIGEN_COLUMN(Matrix, m))

  //using TestRecHitSoA = TestRecHitSoALayout<>;

  // Generate structure of arrays (SoA) layout with RecHit dataformat
  GENERATE_SOA_LAYOUT(TestRecHitSoALayout,
                      // columns: one value per element
                      SOA_COLUMN(int32_t, event),
                      SOA_COLUMN(int32_t, detid),
                      SOA_COLUMN(double , adc),
                      SOA_COLUMN(double , adcm),
                      SOA_COLUMN(double , adc_cm),
                      SOA_COLUMN(double , toa),
                      SOA_COLUMN(double , tot),
                      SOA_COLUMN(double , triglatency)
  )
  using TestRecHitSoA = TestRecHitSoALayout<>;
  
}  // namespace portabletest

#endif  // DataFormats_PortableTestObjects_interface_TestRecHitSoA_h
