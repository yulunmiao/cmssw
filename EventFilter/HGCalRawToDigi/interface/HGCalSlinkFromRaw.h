#ifndef EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h
#define EventFilter_HGCalRawToDigi_HGCalSlinkFromRaw_h

#include "EventFilter/HGCalRawToDigi/interface/HGCalRawDataBaseEmulator.h"
#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw/FileReader.h"

#include <ostream>
#include <iostream>
#include <fstream>


namespace hgcal {

  /// a reader of s-link data from binary files
  class SlinkFromRaw : public SlinkEmulatorBase {

  public:

    /// \param[in] configuration parameters
    explicit SlinkFromRaw(const edm::ParameterSet &iConfig);
    
    FEDRawDataCollection next() override;
    
  private:

    size_t ifile_;
    std::vector<std::string> inputfiles_;
    hgcal_slinkfromraw::FileReader fileReader_;
    hgcal_slinkfromraw::RecordT<4095> *record_;
    uint32_t nEvents_;
  };

}  // namespace hgcal

#endif
