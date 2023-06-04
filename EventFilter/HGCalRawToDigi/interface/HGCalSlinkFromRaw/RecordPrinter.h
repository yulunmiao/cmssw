#ifndef hgcal_slinkfromraw_RecordPrinter_h
#define hgcal_slinkfromraw_RecordPrinter_h

#include <iostream>
#include <iomanip>

#include "RecordConfiguredB.h"
#include "RecordConfiguringA.h"
#include "RecordConfiguringB.h"
#include "RecordContinuing.h"
#include "RecordEnding.h"
#include "RecordHaltingA.h"
#include "RecordHaltingB.h"
#include "RecordInitializing.h"
#include "RecordPausing.h"
#include "RecordResetting.h"
#include "RecordResuming.h"
#include "RecordRunning.h"
#include "RecordStarting.h"
#include "RecordStopping.h"

namespace hgcal_slinkfromraw {

  void RecordPrinter(const Record *h, std::ostream &o=std::cout, std::string s="") {
    if     (h->state()==FsmState::ConfiguredB ) ((const RecordConfiguredB* )h)->print(o,s);
    else if(h->state()==FsmState::ConfiguringA) ((const RecordConfiguringA*)h)->print(o,s);
    else if(h->state()==FsmState::ConfiguringB) ((const RecordConfiguringB*)h)->print(o,s);
    else if(h->state()==FsmState::Continuing  ) ((const RecordContinuing*  )h)->print(o,s);
    else if(h->state()==FsmState::Ending      ) ((const RecordEnding*      )h)->print(o,s);
    else if(h->state()==FsmState::HaltingA    ) ((const RecordHaltingA*    )h)->print(o,s);
    else if(h->state()==FsmState::HaltingB    ) ((const RecordHaltingB*    )h)->print(o,s);
    else if(h->state()==FsmState::Initializing) ((const RecordInitializing*)h)->print(o,s);
    else if(h->state()==FsmState::Pausing     ) ((const RecordPausing*     )h)->print(o,s);
    else if(h->state()==FsmState::Resetting   ) ((const RecordResetting*   )h)->print(o,s);
    else if(h->state()==FsmState::Resuming    ) ((const RecordResuming*    )h)->print(o,s);
    else if(h->state()==FsmState::Running     ) ((const RecordRunning*     )h)->print(o,s);
    else if(h->state()==FsmState::Starting    ) ((const RecordStarting*    )h)->print(o,s);
    else if(h->state()==FsmState::Stopping    ) ((const RecordStopping*    )h)->print(o,s);
    else h->print(o,s);
  }

  void RecordPrinter(const Record &h, std::ostream &o=std::cout, std::string s="") {
    RecordPrinter(&h,o,s);
  }
}

#endif
