#ifndef hgcal_slinkfromraw_RelayReader_h
#define hgcal_slinkfromraw_RelayReader_h

#include <iostream>
#include <fstream>
#include <sstream>

#include "FileReader.h"
#include "RecordPrinter.h"

namespace hgcal_slinkfromraw {

  class RelayReader : public FileReader {
  public:
    
    RelayReader() {
      _runActive=false;
    }
    
    bool read(Record *r) {
      if(!_runActive) {
	if(!FileReader::read(r)) return false;
	
	if(r->state()==FsmState::Starting) {
	  _runActive=true;
	  _runRecord=0;
	  
	  const RecordStarting *rs((const RecordStarting*)r);

	  RecordStarting rs2;
	  for(unsigned i(0);i<2;i++) {
	    _runReader[i].openRun(rs->runNumber(),i);

	    if(_runReader[i].read(&rs2)) {
	      assert(rs2.runNumber() ==rs->runNumber());
	      assert(rs2.maxEvents() ==rs->maxEvents());
	      assert(rs2.maxSeconds()==rs->maxSeconds());
	      assert(rs2.maxSpills() ==rs->maxSpills());
	    } else {
	      _runActive=false;
	    }
	  }

	  return true;
	}

      } else {
	if(_runReader[(_runRecord++)%2].read(r)) {
	  
	  if(r->state()==FsmState::Stopping) {
	    _runActive=false;
	    _runReader[0].close();
	    _runReader[1].close();
	    
	    return FileReader::read(r);
	  }

	} else {
	  return false;
	}
      }
      return true;
    }
    
  protected:
    bool _runActive;
    unsigned _runRecord;
    FileReader _runReader[2];
  };
}

#endif
