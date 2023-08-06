#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw.h"

// example reader by P.Dauncey, using https://gitlab.cern.ch/pdauncey/hgcal10glinkreceiver

using namespace hgcal;

SlinkFromRaw::SlinkFromRaw(const edm::ParameterSet &iConfig) : SlinkEmulatorBase(iConfig) {
  inputfiles_ = iConfig.getUntrackedParameter<std::vector<std::string>>("inputs");
  inputfiles_trg_ = iConfig.getUntrackedParameter<std::vector<std::string>>("trig_inputs");
  ifile_ = 0;

  if (!inputfiles_trg_.empty()) {
    // `inputfiles_` and `inputfiles_trg_` must be 1-to-1 matched
    assert(inputfiles_.size() == inputfiles_trg_.size());
  }

  edm::LogInfo("SlinkFromRaw") << "files: \n";
  copy(begin(inputfiles_), end(inputfiles_), std::ostream_iterator<std::string>{std::cout, "\n"});

  edm::LogInfo("SlinkFromRaw") << "trig files: \n";
  copy(begin(inputfiles_trg_), end(inputfiles_trg_), std::ostream_iterator<std::string>{std::cout, "\n"});

  // Make the buffer space for the records
  record_ = new hgcal_slinkfromraw::RecordT<4095>;
  record_trg_ = new hgcal_slinkfromraw::RecordT<4095>;
  nEvents_ = 0;
}

//
std::unique_ptr<FEDRawDataCollection> SlinkFromRaw::next() {
  //open for the first time
  if (fileReader_.closed()) {
    fileReader_.open(inputfiles_[ifile_]);

    // open the corresponding trigger file
    if (!inputfiles_trg_.empty()) {
      fileReader_trg_.close();
      fileReader_trg_.open(inputfiles_trg_[ifile_]);
    }
  }

  //no more records in the file, move to next
  if (!fileReader_.read(record_)) {
    fileReader_.close();

    ifile_++;
    if (ifile_ >= inputfiles_.size())
      throw cms::Exception("FileReadError") << "no more files";

    return next();
  }

  //if record is stop or starting read again
  if (record_->state() == hgcal_slinkfromraw::FsmState::Stopping) {
    edm::LogInfo("SlinkFromRaw") << "RecordStopping will search for next";
    const hgcal_slinkfromraw::RecordStopping *rStop((hgcal_slinkfromraw::RecordStopping *)record_);
    rStop->print();
    return next();
  }
  if (record_->state() == hgcal_slinkfromraw::FsmState::Starting) {
    edm::LogInfo("SlinkFromRaw") << "RecordStarting will search for next";
    const hgcal_slinkfromraw::RecordStarting *rStart((hgcal_slinkfromraw::RecordStarting *)record_);
    rStart->print();
    return next();
  }
  if (record_->state() == hgcal_slinkfromraw::FsmState::Continuing) {
    edm::LogInfo("SlinkFromRaw") << "RecordContinuing";
    const hgcal_slinkfromraw::RecordContinuing *rCont((hgcal_slinkfromraw::RecordContinuing *)record_);
    rCont->print();
    return next();
  }

  //analyze event
  auto nextTrgEvent = [&]() {
    const hgcal_slinkfromraw::RecordRunning *rTrgEvent = nullptr;

    if (!fileReader_trg_.closed()) {
      fileReader_trg_.read(record_trg_);

      if (record_trg_->state() == hgcal_slinkfromraw::FsmState::Stopping) {
        const hgcal_slinkfromraw::RecordStopping *rStop((hgcal_slinkfromraw::RecordStopping *)record_trg_);
        std::cout << "[Trigger Link]";
        rStop->print();
        return rTrgEvent;
      }
      if (record_trg_->state() == hgcal_slinkfromraw::FsmState::Starting) {
        const hgcal_slinkfromraw::RecordStarting *rStart((hgcal_slinkfromraw::RecordStarting *)record_trg_);
        std::cout << "[Trigger Link]";
        rStart->print();
        fileReader_trg_.read(record_trg_);
      }
      if (record_trg_->state() == hgcal_slinkfromraw::FsmState::Continuing) {
        const hgcal_slinkfromraw::RecordContinuing *rCont((hgcal_slinkfromraw::RecordContinuing *)record_trg_);
        std::cout << "[Trigger Link]";
        rCont->print();
        fileReader_trg_.read(record_trg_);
      }

      rTrgEvent = ((hgcal_slinkfromraw::RecordRunning *)record_trg_);
      if (!rTrgEvent->valid())
        throw cms::Exception("[HGCalSlinkFromRaw::next]") << "trg record running is invalid";
    }
    return rTrgEvent;
  };

  edm::LogInfo("SlinkFromRaw: Reading record from file #") << ifile_ << "nevents=" << nEvents_ << "\n";
  const hgcal_slinkfromraw::RecordRunning *rEvent((hgcal_slinkfromraw::RecordRunning *)record_);
  if (!rEvent->valid())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "record running is invalid";
  nEvents_++;

  const hgcal_slinkfromraw::RecordRunning *rTrgEvent = nextTrgEvent();

  bool print(nEvents_ <= 1);
  if (print) {
    rEvent->print();
    if (rTrgEvent) {
      std::cout << "[Trigger Link]";
      rTrgEvent->print();
    }
  }

  //FIXME: these have to be read from the TCDS block
  metaData_.trigType_ = 0;
  metaData_.trigTime_ = 0;
  metaData_.trigWidth_ = 0;

  while (rTrgEvent) {
    if (rTrgEvent->slinkBoe()->eventId() < rEvent->slinkBoe()->eventId()) {
      rTrgEvent = nextTrgEvent();
      continue;
    }
    if (rTrgEvent->slinkBoe()->eventId() == rEvent->slinkBoe()->eventId() &&
        rTrgEvent->slinkEoe()->bxId() == rEvent->slinkEoe()->bxId() &&
        rTrgEvent->slinkEoe()->orbitId() == rEvent->slinkEoe()->orbitId()) {
      metaData_.trigType_ = rTrgEvent->slinkBoe()->l1aType();
      readTriggerData(rTrgEvent);
      break;
    }
  }

  // Access the Slink header ("begin-of-event")
  const hgcal_slinkfromraw::SlinkBoe *b(rEvent->slinkBoe());
  assert(b != nullptr);
  if (!b->validPattern())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "SlinkBoe has invalid pattern";

  // Access the Slink trailer ("end-of-event")
  const hgcal_slinkfromraw::SlinkEoe *e(rEvent->slinkEoe());
  assert(e != nullptr);
  if (!e->validPattern())
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "SlinkEoe has invalid pattern";

  // Access the BE packet header
  const hgcal_slinkfromraw::BePacketHeader *bph(rEvent->bePacketHeader());
  if (bph == nullptr)
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "Null pointer to BE packet header";

  // Access ECON-D packet as an array of 32-bit words
  const uint32_t *pEcond(rEvent->econdPayload());
  if (pEcond == nullptr)
    throw cms::Exception("[HGCalSlinkFromRaw::next]") << "Null pointer to ECON-D payload";

  //get payload and its length
  auto *payload = record_->getPayload();
  auto payloadLength = record_->payloadLength();

  //  for(auto i=0; i<=payloadLength; i++)
  //    std::cout <<std::dec << i << "  " << std::hex << std::setfill('0') << payload[i] << std::endl;

  //NOTE these were hacks for Paul's file which reverts the
  //ECOND pseudo-endianness wrt to capture block and s-link
  //so we invert the first 3 64b word (s-link + capture block)
  //unclear how the final system will be
  //payloadLength-=2;
  // for(auto i=0; i<payloadLength; i++) {
  //   payload[i]=((payload[i]&0xffffffff)<<32) | payload[i]>>32;
  // }

  //put in the event (last word is a 0xdeadbeefdeadbeef which can be disregarded)
  auto raw_data = std::make_unique<FEDRawDataCollection>();
  size_t total_event_size = (payloadLength - 1) * sizeof(uint64_t) / sizeof(char);
  auto &fed_data = raw_data->FEDData(1);  //data for one FED
  fed_data.resize(total_event_size);
  auto *ptr = fed_data.data();
  memcpy(ptr, (char *)payload, total_event_size);

  return raw_data;
}

void SlinkFromRaw::readTriggerData(const hgcal_slinkfromraw::RecordRunning *rTrgEvent) {
  constexpr uint64_t pkt_mask = 0xff;
  constexpr uint64_t pkt_sep = 0xfecafecafecafe;

  // TODO: use implementations from std <bit> in c++20
  constexpr auto countl_zero = [](uint32_t input) -> unsigned char {
    if (input == 0) {
      return 32;
    }
    constexpr uint32_t highestBit = 1 << 31;
    unsigned char result = 0;
    for (; (input & highestBit) == 0; input <<= 1) {
      ++result;
    }
    return result;
  };

  constexpr auto countr_zero = [](uint32_t input) -> unsigned char {
    if (input == 0) {
      return 32;
    }
    unsigned char result = 0;
    for (; (input & 1) == 0; input >>= 1) {
      ++result;
    }
    return result;
  };

  if (rTrgEvent && rTrgEvent->payloadLength() > 0) {
    auto p = (const uint64_t *)rTrgEvent;
    int length = 0;
    p += 4;  // (1 record header + 2 slink header + 1 trigger readout header)
    for (unsigned iblock = 0; iblock < 4 && p < (const uint64_t *)rTrgEvent + rTrgEvent->payloadLength(); ++iblock) {
      LogDebug("SlinkFromRaw") << "Header: " << std::hex << std::setfill('0') << "0x" << *p << std::endl;
      assert((*p >> 8) == pkt_sep);
      length = *p & pkt_mask;
      if (iblock == 2) {
        // scintillator
        // the length should be 9 (BX) * 5 (64b word)
        // only the 1st 64b word is used; the last (5th) word is a separator
        auto p_scint = p + 1;
        uint32_t trigtime = 0;
        uint32_t trigwidth = 0;
        bool triggered = false;
        while (p_scint <= p + length) {
          // Bits [31:  0] : External Trigger
          // Bits [63: 32] : 0xABCDFEED
          assert((*p_scint >> 32) == 0xABCDFEED);
          uint32_t trigbits = *p_scint & 0xFFFFFFFF;
          LogDebug("SlinkFromRaw") << "BX " << (p_scint - p) / 5 << ": " << std::hex << std::setfill('0') << "0x"
                                        << *p_scint << ", trigbits = "
                                        << "0x" << trigbits << std::endl;
          if (not triggered) {
            trigtime += countl_zero(trigbits);
            if (trigbits > 0) {
              // first BX with the trigger fired
              triggered = true;
              // count the 1s from the right
              trigwidth += countr_zero(~trigbits);
            }
          } else {
            // trigger already fired in previous BX
            if (trigbits > 0) {
              // trigger signal extends more than 1 BX
              // count the 1s from the left
              trigwidth += countl_zero(~trigbits);
            } else if (trigbits == 0) {
              // stop processing when the trigger is no longer fired
              break;
            }
          }
          p_scint += 5;
        }
        LogDebug("SlinkFromRaw") << "==> trigtime = " << std::dec << std::setfill(' ') << trigtime
                                      << ", trigwidth = " << trigwidth << std::endl;
        metaData_.trigTime_ = trigtime;
        metaData_.trigWidth_ = trigwidth;
        break;
      }
      p += length + 1;
    }
  }
}
