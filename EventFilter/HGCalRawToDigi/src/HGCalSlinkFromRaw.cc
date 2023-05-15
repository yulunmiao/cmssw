#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

using namespace hgcal;

//
SlinkFromRaw::SlinkFromRaw(const edm::ParameterSet &iConfig) : SlinkEmulatorBase(iConfig) {

  std::vector<std::string> inputfile_list=iConfig.getUntrackedParameter<std::vector<std::string>>("inputs");
  
  throw cms::Exception("SlinkFromRaw::CTOR") << "Not implemented!";
}

//
FEDRawDataCollection SlinkFromRaw::next() {
  throw cms::Exception("SlinkFromRaw::next") << "Not implemented!";
}
