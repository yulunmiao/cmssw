/****************************************************************************
 *
 * This is a part of HGCal offline software.
 * Author:
 *   Laurent Forthomme
 *
 ****************************************************************************/

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableGenericConfigRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableGenericConfig.h"

class HGCalConfigESSourceFromYAMLAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit HGCalConfigESSourceFromYAMLAnalyzer(const edm::ParameterSet& iConfig)
      : tokenConds_(esConsumes<HGCalCondSerializableGenericConfig, HGCalCondSerializableGenericConfigRcd>(
            edm::ESInputTag(iConfig.getParameter<std::string>("label")))) {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("label", {});
    descriptions.addWithDefaultLabel(desc);
  }

private:
  void analyze(const edm::Event&, const edm::EventSetup& iSetup) override {
    // get timing calibration parameters
    if (cfgWatcher_.check(iSetup))
      edm::LogInfo("HGCalConfigESSourceFromYAMLAnalyzer") << "Conditions retrieved:\n" << iSetup.getData(tokenConds_);
  }

  edm::ESWatcher<HGCalCondSerializableGenericConfigRcd> cfgWatcher_;
  edm::ESGetToken<HGCalCondSerializableGenericConfig, HGCalCondSerializableGenericConfigRcd> tokenConds_;
};

DEFINE_FWK_MODULE(HGCalConfigESSourceFromYAMLAnalyzer);
