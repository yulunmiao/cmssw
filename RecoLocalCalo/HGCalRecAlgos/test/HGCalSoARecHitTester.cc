#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHostCollection.h"
#include "DataFormats/HGCalRecHit/interface/HGCalRecHitHostCollection.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializableSiCellChannelInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiCellChannelInfo.h"
#include "Geometry/HGCalMapping/interface/HGCalElectronicsMappingTools.h"

#include <iostream>
#include <algorithm>


class HGCalSoARecHitTester : public edm::one::EDAnalyzer<edm::one::WatchRuns> {

public:
  
  explicit HGCalSoARecHitTester(const edm::ParameterSet& iConfig)
    : digisToken_(consumes<hgcaldigi::HGCalDigiHostCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
      rechitsToken_(consumes<hgcalrechit::HGCalRecHitHostCollection>(iConfig.getParameter<edm::InputTag>("RecHits"))),
      moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd,edm::Transition::BeginRun>(iConfig.getParameter<edm::ESInputTag>("ModuleInfo"))),
      siModuleInfoToken_(esConsumes<HGCalCondSerializableSiCellChannelInfo,HGCalCondSerializableSiCellChannelInfoRcd,edm::Transition::BeginRun>(iConfig.getParameter<edm::ESInputTag>("SiModuleInfo")))
  {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("Digis",edm::InputTag("hgcalDigis"));
    desc.add<edm::InputTag>("RecHits",edm::InputTag("hgcalRecHit"));
    desc.add<edm::ESInputTag>("ModuleInfo",edm::ESInputTag(""));
    desc.add<edm::ESInputTag>("SiModuleInfo",edm::ESInputTag(""));
    descriptions.addWithDefaultLabel(desc);
  }

private:

  void analyze(const edm::Event&, const edm::EventSetup& iSetup) override;
  void beginRun(edm::Run const& , edm::EventSetup const&) override;
  void endRun(edm::Run const& , edm::EventSetup const&) override {};
  
  //tokens to access collections in ROOT file
  const edm::EDGetTokenT<hgcaldigi::HGCalDigiHostCollection> digisToken_;
  const edm::EDGetTokenT<hgcalrechit::HGCalRecHitHostCollection> rechitsToken_;
  const edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  const edm::ESGetToken<HGCalCondSerializableSiCellChannelInfo,HGCalCondSerializableSiCellChannelInfoRcd> siModuleInfoToken_;

  //electronics ID to det ID map
  std::map<uint32_t,uint32_t> ele2detid_;
};


//
void HGCalSoARecHitTester::beginRun(edm::Run const& iRun, const edm::EventSetup& iSetup) {

  //get module and silicon cell mapping
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  auto siCellInfo = iSetup.getData(siModuleInfoToken_);
  ele2detid_=hgcal::mapSiGeoToElectronics(moduleInfo,siCellInfo,false);
  edm::LogInfo("HGCalSoARecHitTester") << "Retrieved ele->det mapping with " << ele2detid_.size() << " entries from event setup" << std::endl;
}

//
void HGCalSoARecHitTester::analyze(const edm::Event &iEvent, const edm::EventSetup& iSetup) {

  //retrieve digis and rechits
  const auto& digis = iEvent.get(digisToken_);
  auto const& digis_view = digis.const_view();
  int32_t ndigis=digis_view.metadata().size();
  
  const auto& rechits = iEvent.get(rechitsToken_);
  auto const& rechits_view = rechits.const_view();
  int32_t nhits=rechits_view.metadata().size();

  //check ElectronicsId to DetId mapping
  uint32_t ndetidFound(0);
  for(int32_t i = 0; i < digis_view.metadata().size(); ++i) {
    auto digi = digis_view[i];
    uint32_t eleid=digi.electronicsId();
    ndetidFound += (ele2detid_.count(eleid)==1);
  }

  //report
  std::cout << "[HGCalSoARecHitTester]" << std::endl
            << "ndigis=" << ndigis << " nhits=" << nhits << std::endl
            << "can assign " << ndetidFound << " detIds to DIGIS from mapping" << std::endl;
  
}


DEFINE_FWK_MODULE(HGCalSoARecHitTester);
