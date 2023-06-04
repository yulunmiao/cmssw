#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"


#include "EventFilter/HGCalRawToDigi/interface/HGCalSlinkFromRaw/FileReader.h"

// example reader by P.Dauncey, using https://gitlab.cern.ch/pdauncey/hgcal10glinkreceiver

using namespace hgcal;

SlinkFromRaw::SlinkFromRaw(const edm::ParameterSet &iConfig) : SlinkEmulatorBase(iConfig) {

    std::vector<std::string> inputfile_list=iConfig.getUntrackedParameter<std::vector<std::string>>("inputs");
    
    edm::LogInfo("SlinkFromRaw") << "files: \n";
    copy(begin(inputfile_list),end(inputfile_list),
         std::ostream_iterator<std::string>{std::cout,"\n"});

    for (const auto& inputfile : inputfile_list){
        // Create the file reader
        hgcal_slinkfromraw::FileReader _fileReader;

        // Make the buffer space for the records
        hgcal_slinkfromraw::RecordT<4095> *r(new hgcal_slinkfromraw::RecordT<4095>);
        
        // Set up specific records to interpet the formats
        const hgcal_slinkfromraw::RecordStarting *rStart((hgcal_slinkfromraw::RecordStarting*)r);
        const hgcal_slinkfromraw::RecordStopping *rStop ((hgcal_slinkfromraw::RecordStopping*)r);
        const hgcal_slinkfromraw::RecordRunning  *rEvent((hgcal_slinkfromraw::RecordRunning*) r);
        
        // Defaults to the files being in directory "dat"
        // Can call setDirectory("blah") to change this
        //_fileReader.setDirectory("somewhere/else");

        edm::LogInfo("SlinkFromRaw: Reading file") << inputfile << "\n";
        _fileReader.open(inputfile);

        unsigned nEvents(0);
        
        while(_fileReader.read(r)) {
            if(       r->state()==hgcal_slinkfromraw::FsmState::Starting) {
                rStart->print();
                std::cout << std::endl;
                
            } else if(r->state()==hgcal_slinkfromraw::FsmState::Stopping) {
                rStop->print();
                std::cout << std::endl;
                
            } else {
                
                // We have an event record
                nEvents++;
                
                bool print(nEvents<=1);
                
                if(print) {
                    rEvent->print();
                    std::cout << std::endl;
                }
                
                // Check id is correct
                if(!rEvent->valid()) rEvent->print();
                
                // Access the Slink header ("begin-of-event")
                // This should always be present; check pattern is correct
                const hgcal_slinkfromraw::SlinkBoe *b(rEvent->slinkBoe());
                assert(b!=nullptr);
                if(!b->validPattern()) b->print();
                
                // Access the Slink trailer ("end-of-event")
                // This should always be present; check pattern is correct
                const hgcal_slinkfromraw::SlinkEoe *e(rEvent->slinkEoe());
                assert(e!=nullptr);
                if(!e->validPattern()) e->print();
                
                // Access the BE packet header
                const hgcal_slinkfromraw::BePacketHeader *bph(rEvent->bePacketHeader());
                if(bph!=nullptr && print) bph->print();
                
                // Access ECON-D packet as an array of 32-bit words
                const uint32_t *pEcond(rEvent->econdPayload());
                
                // Check this is not an empty event
                if(pEcond!=nullptr) {
                    
                    if(print) {
                        std::cout << "First 10 words of ECON-D packet" << std::endl;
                        std::cout << std::hex << std::setfill('0');
                        for(unsigned i(0);i<10;i++) {
                            std::cout << "0x" << std::setw(8) << pEcond[i] << std::endl;
                        }
                        std::cout << std::dec << std::setfill(' ');
                        std::cout << std::endl;
                    }
                    
                }
            }
        }
        
        std::cout << "Total number of event records seen = "
                  << nEvents << std::endl;
        
        delete r;
        
    }


  throw cms::Exception("SlinkFromRaw::CTOR") << "Not implemented!";
}

//
FEDRawDataCollection SlinkFromRaw::next() {
  throw cms::Exception("SlinkFromRaw::next") << "Not implemented!";
}
