#ifndef hgcal_slinkfromraw_RecordHaltingB_h
#define hgcal_slinkfromraw_RecordHaltingB_h

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Record.h"

namespace hgcal_slinkfromraw {

  class RecordHaltingB : public RecordT<2> {
  
  public:
    RecordHaltingB() {
    }
    
    void setHeader(uint32_t t=time(0)) {
      setState(FsmState::HaltingB);
      setPayloadLength(2);
      setUtc(t);
    }

    bool valid() const {
      return validPattern() && state()==FsmState::HaltingB;
    }
    
    uint32_t relayNumber() const {
      return _payload[0]&0xffffffff;
    }

    uint32_t configurationNumber() const {
      return _payload[0]>>32;
    }

    uint32_t numberOfRuns() const {
      return _payload[1]&0xffffffff;
    }

    uint32_t numberOfEvents() const {
      return _payload[1]>>32;
    }

    void setRelayNumber(uint32_t n) {
      _payload[0]&=0xffffffff00000000;
      _payload[0]|=n;
    }
   
    void setConfigurationNumber(uint32_t n) {
      _payload[0]&=0x00000000ffffffff;
      _payload[0]|=uint64_t(n)<<32;
    }
   
    void setNumberOfRuns(uint32_t n) {
      _payload[1]&=0xffffffff00000000;
      _payload[1]|=n;
    }
   
    void setNumberOfEvents(uint32_t n) {
      _payload[1]&=0x00000000ffffffff;
      _payload[1]|=uint64_t(n)<<32;
    }
   
    void print(std::ostream &o=std::cout, std::string s="") const {
      o << s << "RecordHaltingB::print()" << std::endl;
      RecordHeader::print(o,s+" ");
      
      for(unsigned i(0);i<payloadLength();i++) {
	o << s << "   Payload word " << std::setw(5) << " = 0x"
	  << std::hex << std::setfill('0')
	  << std::setw(16) << _payload[i]
	  << std::dec << std::setfill(' ') << std::endl;
      }

      o << s << "  Relay number         = "
	<< std::setw(10) << relayNumber() << std::endl;
      o << s << "  Configuration number = "
	<< std::setw(10) << configurationNumber() << std::endl;
      o << s << "  Number of runs       = "
	<< std::setw(10) << numberOfRuns() << std::endl;
      o << s << "  Number of events     = "
	<< std::setw(10) << numberOfEvents() << std::endl;
    }

  private:
  };

}

#endif
