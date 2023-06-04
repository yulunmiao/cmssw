#ifndef hgcal_slinkfromraw_RecordPausing_h
#define hgcal_slinkfromraw_RecordPausing_h

#include <iostream>
#include <iomanip>
#include <cassert>

#include "Record.h"

namespace hgcal_slinkfromraw {

  class RecordPausing : public Record {
  
  public:
    RecordPausing() {
    }
    
    bool valid() const {
      return validPattern() && state()==FsmState::Pausing;
    }
    
    void setHeader(uint32_t t=time(0)) {
      setState(FsmState::Pausing);
      setPayloadLength(0);
      setUtc(t);
    }

    void print(std::ostream &o=std::cout, std::string s="") const {
      o << s << "RecordPausing::print()" << std::endl;
      RecordHeader::print(o,s+" ");
    }
    
  private:
  };

}

#endif
