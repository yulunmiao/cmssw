#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw.h"

// example reader by P.Dauncey, using https://gitlab.cern.ch/pdauncey/hgcal10glinkreceiver

using namespace hgcal;

SlinkFromRaw::SlinkFromRaw(const edm::ParameterSet &iConfig) : SlinkEmulatorBase(iConfig) {
 
  inputfiles_=iConfig.getUntrackedParameter<std::vector<std::string>>("inputs");
  ifile_=0;
    
  edm::LogInfo("SlinkFromRaw") << "files: \n";
  copy(begin(inputfiles_),end(inputfiles_), std::ostream_iterator<std::string>{std::cout,"\n"});
    
  // Make the buffer space for the records
  record_ = new hgcal_slinkfromraw::RecordT<4095>;
  nEvents_=0;
}

//
FEDRawDataCollection SlinkFromRaw::next() {

  FEDRawDataCollection raw_data;

  //open for the first time
  if( fileReader_.closed() ) {
    auto inputfile = inputfiles_[ifile_];
    fileReader_.open(inputfile);    
  }

  //no more records in the file, move to next
  if(!nextRecord()) {
    ifile_++;
  
    if(ifile_>=inputfiles_.size())
      throw cms::Exception("[HGCalSlinkFromRaw::next]") << "no more files";
   
    fileReader_.close();
    auto inputfile=inputfiles_[ifile_];
    fileReader_.open(inputfile);

    return next();      
  }
  
  //if record is stop or starting read again
  if(record_->state()==hgcal_slinkfromraw::FsmState::Stopping){
    edm::LogInfo("SlinkFromRaw") << "RecordStopping will search for next";
    const hgcal_slinkfromraw::RecordStopping *rStop((hgcal_slinkfromraw::RecordStopping*)record_);
    rStop->print();
    return next();
  }
  if(record_->state()==hgcal_slinkfromraw::FsmState::Starting) {
    edm::LogInfo("SlinkFromRaw") << "RecordStarting will search for next";
    const hgcal_slinkfromraw::RecordStarting *rStart((hgcal_slinkfromraw::RecordStarting*)record_);
    rStart->print();
    return next();
  }

  //analyze event
  edm::LogInfo("SlinkFromRaw: Reading record from file #") << ifile_ << "nevents=" << nEvents_ << "\n";
  const hgcal_slinkfromraw::RecordRunning  *rEvent((hgcal_slinkfromraw::RecordRunning*)record_);
  if(!rEvent->valid())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "record running is invalid";
  nEvents_++;
  bool print(nEvents_<=1);
  if(print) {     
    rEvent->print();
  }

  //FIXME: these have to be read from the TCDS block
  metaData_.trigType_=0;
  metaData_.trigTime_=0;
  metaData_.trigWidth_=0;
    
  // Access the Slink header ("begin-of-event")
  const hgcal_slinkfromraw::SlinkBoe *b(rEvent->slinkBoe());
  assert(b!=nullptr);
  if(!b->validPattern())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "SlinkBoe has invalid pattern";
  uint32_t sourceId=b->sourceId();
  
  // Access the Slink trailer ("end-of-event")
  const hgcal_slinkfromraw::SlinkEoe *e(rEvent->slinkEoe());
  assert(e!=nullptr);
  if(!e->validPattern())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "SlinkEoe has invalid pattern";
  
  // Access the BE packet header
  const hgcal_slinkfromraw::BePacketHeader *bph(rEvent->bePacketHeader());
  if(bph==nullptr)
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "Null pointer to BE packet header";
  
  // Access ECON-D packet as an array of 32-bit words
  const uint32_t *pEcond(rEvent->econdPayload());
  if(pEcond==nullptr)
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "Null pointer to ECON-D payload";
          
  //copy to the event      
  auto *payload=record_->getPayload();
  auto payloadLength=record_->payloadLength()-2;

  //this is a hack to store 32b as [MSB 32b | LSB 32b]
  //unclear how the final system will be
  for(auto i=0; i<payloadLength; i++) {
    std::cout << "0x" << std::hex << (payload[i]>>32) << "\t" << (payload[i] & 0xffffffff) << std::endl;
    //    if(i!=3)
    payload[i]=((payload[i]&0xffffffff)<<32) | payload[i]>>32;
  }
  size_t total_event_size = payloadLength/sizeof(char);
  std::cout << "Total event size in bytes is:" << total_event_size << " sourceId=" << sourceId << std::endl;
  auto& fed_data = raw_data.FEDData(1); //data for one FED
  fed_data.resize(total_event_size);
  auto* ptr = fed_data.data();
  memcpy(ptr, (char*)payload, total_event_size);
  

  return raw_data;
}
  
  
