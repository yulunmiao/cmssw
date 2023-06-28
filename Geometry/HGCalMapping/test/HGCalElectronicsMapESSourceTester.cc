#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializableSiCellChannelInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiCellChannelInfo.h"

#include <iostream>

/**
   @short a tester for the logical mapping as ESsource
 */
class HGCalElectronicsMapESSourceTester : public edm::one::EDAnalyzer<edm::one::WatchRuns> {

public:

  /**
     @short constructor
   */
  explicit HGCalElectronicsMapESSourceTester(const edm::ParameterSet& iConfig)
    :  moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd,edm::Transition::BeginRun>(iConfig.getParameter<edm::ESInputTag>("ModuleInfo"))),
       siModuleInfoToken_(esConsumes<HGCalCondSerializableSiCellChannelInfo,HGCalCondSerializableSiCellChannelInfoRcd,edm::Transition::BeginRun>(iConfig.getParameter<edm::ESInputTag>("SiModuleInfo")))
  {}

  /**
     @short description filler
   */
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::ESInputTag>("ModuleInfo",edm::ESInputTag(""));
    desc.add<edm::ESInputTag>("SiModuleInfo",edm::ESInputTag(""));
    descriptions.addWithDefaultLabel(desc);
  }
  
private:

  /**
     @short check what has been put in the event setup at start of the run
   */
  void beginRun(edm::Run const& iEvent, edm::EventSetup const&) override;

  /**
     @short do something event-by-event
   */
  void analyze(const edm::Event&, const edm::EventSetup& iSetup) override {}

  /**
     @short do another thing end of the run
  */
  void endRun(edm::Run const& iEvent, edm::EventSetup const&) override {}

  /**
     @short do something end of the job
  */
  void endJob() override {}
  
  //tokens and record watches
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  edm::ESGetToken<HGCalCondSerializableSiCellChannelInfo,HGCalCondSerializableSiCellChannelInfoRcd> siModuleInfoToken_;
};

//
void HGCalElectronicsMapESSourceTester::beginRun(edm::Run const& iRun, const edm::EventSetup& iSetup) {
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  auto siCellInfo = iSetup.getData(siModuleInfoToken_);

  for(auto m : moduleInfo.params_){
    if(m.isSiPM) continue;    
    bool isHD=m.isHD;
    uint8_t wafType=m.wafType;
    std::vector<HGCalSiCellChannelInfo> cells=siCellInfo.getAllCellsInModule(isHD,wafType);
    std::cout << isHD << " " << (int) wafType << " has " << cells.size() << std::endl;
  }

  std::cout << "Read module info with " << moduleInfo.params_.size() << " entries" << std::endl
            << "Read si cell info with " << siCellInfo.params_.size() << " entries" << std::endl;

  
}


DEFINE_FWK_MODULE(HGCalElectronicsMapESSourceTester);
