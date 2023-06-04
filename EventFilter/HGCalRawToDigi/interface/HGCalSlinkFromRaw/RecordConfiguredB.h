#ifndef hgcal_slinkfromraw_RecordConfiguredB_h
#define hgcal_slinkfromraw_RecordConfiguredB_h

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Record.h"

namespace hgcal_slinkfromraw {

  class RecordConfiguredB : public RecordT<511> {
  
  public:
    RecordConfiguredB() {
    }
    
    void setHeader(uint32_t c) {
      setState(FsmState::ConfiguredB);
      setPayloadLength(2);
      setUtc(c);
    }
  
    bool valid() const {
      return validPattern() && state()==FsmState::ConfiguredB;
    }
    
    uint32_t relayNumber() const {
      return _payload[0]&0xffffffff;
    }

    uint32_t configurationCounter() const {
      return _payload[0]>>32;
    }

    const char* configurationPacketHeader() const {
      return (const char*)(_payload+1);
      //for(unsigned j(1);j<payloadLength() && j<2;j++) {
      //}
    }

    void setRelayNumber(uint32_t t=time(0)) {
      _payload[0]&=0xffffffff00000000;
      _payload[0]|=t;
    }
   
    void setConfigurationCounter(uint32_t c) {
      _payload[0]&=0x00000000ffffffff;
      _payload[0]|=uint64_t(c)<<32;
    }

    void setConfigurationPacketHeader(const std::string &s) {
      unsigned n((s.size()+8)/8);
      assert(n==1);
      std::memcpy(_payload+payloadLength(),s.c_str(),s.size()+1);
      incrementPayloadLength(n);
    }
     
    void setConfigurationPacketValue(uint64_t v) {
      assert(payloadLength()<maxNumberOfPayloadWords());
      _payload[payloadLength()]=v;
      incrementPayloadLength(1);
    }
     
    void print(std::ostream &o=std::cout, std::string s="") const {
      o << s << "RecordConfiguredB::print()" << std::endl;
      RecordHeader::print(o,s+" ");
      
      for(unsigned i(0);i<payloadLength();i++) {
	o << s << "  Payload word " << std::setw(5) << " = 0x"
	  << std::hex << std::setfill('0')
	  << std::setw(16) << _payload[i]
	  << std::dec << std::setfill(' ') << std::endl;
      }
      
      o << s << "  Relay number                = "
	<< std::setw(10) << relayNumber() << std::endl;
      o << s << "  Configuration counter       = "
        << std::setw(10) << configurationCounter() << std::endl;
      o << s << "  Configuration packet header = "
	<< std::setw(10) << std::string(configurationPacketHeader()) << std::endl;
    }

  private:
  };

}

#endif
