#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <iostream>

/**
   @short a tester for the logical mapping as ESsource
 */
class HGCalElectronicsMapESSourceTester : public edm::one::EDAnalyzer<> {

public:
  
  explicit HGCalElectronicsMapESSourceTester(const edm::ParameterSet& iConfig)
    : moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd>(edm::ESInputTag(iConfig.getParameter<std::string>("label")))) {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("label", {});
    descriptions.addWithDefaultLabel(desc);
  }

private:

  /**
     @short the main analysis method to check what has been parsed from the files
   */
  void analyze(const edm::Event&, const edm::EventSetup& iSetup) override;

  //tokens and record watches
  edm::ESWatcher<HGCalCondSerializableModuleInfoRcd> moduleInfoWatcher_;
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
};


//
void HGCalElectronicsMapESSourceTester::analyze(const edm::Event&, const edm::EventSetup& iSetup) {

  if (!moduleInfoWatcher_.check(iSetup)) return;

  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  std::cout << "Read module info with " << moduleInfo.params_.size() << " entries"<< std::endl;

}


DEFINE_FWK_MODULE(HGCalElectronicsMapESSourceTester);
