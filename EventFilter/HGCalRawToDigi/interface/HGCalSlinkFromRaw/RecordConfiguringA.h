#ifndef hgcal_slinkfromraw_RecordConfiguringA_h
#define hgcal_slinkfromraw_RecordConfiguringA_h

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Record.h"

namespace hgcal_slinkfromraw {

  class RecordConfiguringA : public RecordT<9> {
  
  public:
    RecordConfiguringA() {
    }
    
    void setHeader(uint32_t t=time(0)) {
      setState(FsmState::ConfiguringA);
      setPayloadLength(1);
      setUtc(t);
    }

    bool valid() const {
      return validPattern() && state()==FsmState::ConfiguringA;
    }
    
    uint32_t relayNumber() const {
      return _payload[0]&0xffffffff;
    }

    uint32_t maxNumberOfConfigurations() const {
      return _payload[0]>>32;
    }

    uint32_t processorKey(uint32_t id) const {
      for(unsigned j(1);j<payloadLength();j++) {
	if((_payload[j]>>32)==id) return _payload[j]&0xffffffff;
      }
      return 0;
    }

    void setRelayNumber(uint32_t t=time(0)) {
      _payload[0]&=0xffffffff00000000;
      _payload[0]|=t;
    }
   
    void setMaxNumberOfConfigurations(uint32_t n=1) {
      _payload[0]&=0x00000000ffffffff;
      _payload[0]|=uint64_t(n)<<32;
    }
   
    void setProcessKey(uint32_t id, uint32_t key) {
      assert(payloadLength()<maxNumberOfPayloadWords());
      _payload[payloadLength()]=uint64_t(id)<<32|key;
      incrementPayloadLength(1);
    }
   
    void print(std::ostream &o=std::cout, std::string s="") const {
      o << s << "RecordConfiguringA::print()" << std::endl;
      RecordHeader::print(o,s+" ");
      
      for(unsigned i(0);i<payloadLength();i++) {
	o << s << "   Payload word " << std::setw(5) << " = 0x"
	  << std::hex << std::setfill('0')
	  << std::setw(16) << _payload[i]
	  << std::dec << std::setfill(' ') << std::endl;
      }
      
      o << s << "  Relay number                     = "
	<< std::setw(10) << relayNumber() << std::endl;
      o << s << "  Maximum number of configurations = "
	<< std::setw(10) << maxNumberOfConfigurations() << std::endl;

      for(unsigned i(1);i<payloadLength();i++) {
	o << s << "  Process id = 0x"
	  << std::hex << std::setfill('0')
	  << std::setw(8) << (_payload[i]>>32) << ", key = "
	  << std::dec << std::setfill(' ')
	  << std::setw(10) << processorKey(_payload[i]>>32)
	  << std::endl;
      }
    }
  private:
  };

}

#endif
