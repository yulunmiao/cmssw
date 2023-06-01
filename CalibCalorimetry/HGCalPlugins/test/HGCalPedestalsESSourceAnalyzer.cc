#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/libminifloat.h"
#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializablePedestalsRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializablePedestals.h"

class HGCalPedestalsESSourceAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit HGCalPedestalsESSourceAnalyzer(const edm::ParameterSet& iConfig)
      : tokenConds_(esConsumes<HGCalCondSerializablePedestals, HGCalCondSerializablePedestalsRcd>(
            edm::ESInputTag(iConfig.getParameter<std::string>("label")))) {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("label", {});
    descriptions.addWithDefaultLabel(desc);
  }

private:
  
  void analyze(const edm::Event&, const edm::EventSetup& iSetup) override {

    //check if there are new conditions and read them
    if (!cfgWatcher_.check(iSetup)) return;
    auto conds=iSetup.getData(tokenConds_);
    size_t nconds=conds.params_.size();
    edm::LogInfo("HGCalPedestalsESSourceAnalyzer") << "Conditions retrieved:\n" << nconds;

    //print out all conditions readout
    std::cout << "ID\teRx\tROC\tChannel\tIs CM?\tPedestal\tCM slope\tCM offset\tkappa(BX-1)" << std::endl;
    for(auto it : conds.params_) {

      HGCalElectronicsId id(it.first);
      bool cmflag=id.isCM();
      uint32_t eRx=(uint32_t) id.econdeRx();
      uint32_t roc=(uint32_t) eRx/2;
      uint32_t seqch=id.sequentialHalfrocChannel();

      HGCalPedestals table(it.second);
      float pedestal = MiniFloatConverter::float16to32(table.pedestal);
      float cm_slope = MiniFloatConverter::float16to32(table.cm_slope);
      float cm_offset = MiniFloatConverter::float16to32(table.cm_offset);
      float kappa_bxm1 = MiniFloatConverter::float16to32(table.kappa_bxm1);
            
      std::cout << std::hex << id.raw() << " "
                << std::dec << eRx << " " << roc << " " << seqch << " " << cmflag << " "
                << std::setprecision(3)  << " " << pedestal << " " << cm_slope << " " << cm_offset << " " << kappa_bxm1 << std::endl;
    }
    
  }

  edm::ESWatcher<HGCalCondSerializablePedestalsRcd> cfgWatcher_;
  edm::ESGetToken<HGCalCondSerializablePedestals, HGCalCondSerializablePedestalsRcd> tokenConds_;
};

DEFINE_FWK_MODULE(HGCalPedestalsESSourceAnalyzer);
