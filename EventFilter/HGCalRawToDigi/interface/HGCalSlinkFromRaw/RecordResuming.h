#ifndef hgcal_slinkfromraw_RecordResuming_h
#define hgcal_slinkfromraw_RecordResuming_h

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Record.h"

namespace hgcal_slinkfromraw {

  class RecordResuming : public Record {
  
  public:
    RecordResuming() {
    }
    
    bool valid() const {
      return validPattern() && state()==FsmState::Resuming;
    }
    
    void setHeader(uint32_t t=time(0)) {
      setState(FsmState::Resuming);
      setPayloadLength(0);
      setUtc(t);
    }

    void print(std::ostream &o=std::cout, std::string s="") const {
      o << s << "RecordResuming::print()" << std::endl;
      RecordHeader::print(o,s+" ");
    }
    
  private:
  };

}

#endif
