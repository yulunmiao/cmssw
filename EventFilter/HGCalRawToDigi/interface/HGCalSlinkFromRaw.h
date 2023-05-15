#ifndef EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h
#define EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h

#include "EventFilter/HGCalRawToDigi/interface/HGCalRawDataBaseEmulator.h"

namespace hgcal {

  /// a reader of s-link data from binary files
  class SlinkFromRaw : public SlinkEmulatorBase {

  public:

    /// \param[in] configuration parameters
    explicit SlinkFromRaw(const edm::ParameterSet &iConfig);

    /// returns next collection of fed raw data read from the binary file
    FEDRawDataCollection next() override;

  private:
    
    
  };
  
}  // namespace hgcal

#endif
