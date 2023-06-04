#ifndef EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h
#define EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h

#include "EventFilter/HGCalRawToDigi/interface/HGCalRawDataBaseEmulator.h"

#include <ostream>
#include <iostream>
#include <fstream>

namespace hgcal {

  /// a reader of s-link data from binary files
  class SlinkFromRaw : public SlinkEmulatorBase {

  public:

    /// \param[in] configuration parameters
    explicit SlinkFromRaw(const edm::ParameterSet &iConfig);
    
    struct SlinkFromRawEvent {
        unsigned int event, chip;
        int half, bxcounter, eventcounter, orbitcounter;
        std::vector<unsigned int>* daqdata{nullptr};
    };


    FEDRawDataCollection next() override;
    
  private:
    
    
  };
  
}  // namespace hgcal

#endif
