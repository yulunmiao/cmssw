#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/ESGetToken.h"
#include "DataFormats/Math/interface/libminifloat.h"

#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESGetToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ESProducer.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/ModuleFactory.h"
#include "HeterogeneousCore/AlpakaInterface/interface/config.h"
#include "HeterogeneousCore/AlpakaInterface/interface/host.h"
#include "HeterogeneousCore/AlpakaInterface/interface/memory.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializableConfigRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableConfig.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterIndex.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterHostCollection.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/alpaka/HGCalCalibrationParameterDeviceCollection.h"

#include <string>
#include <iostream>

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  namespace hgcalrechit {

    class HGCalConfigurationESProducer : public ESProducer {
    public:

      HGCalConfigurationESProducer(const edm::ParameterSet& iConfig)
        : ESProducer(iConfig),
          charMode_(iConfig.getParameter<int>("charMode")),
          gain_(iConfig.getParameter<int>("gain")) {
        auto cc = setWhatProduced(this,dependsOn(&HGCalConfigurationESProducer::setIndexFromModuleInfo));
        configToken_ = cc.consumes();
        moduleInfoToken_ = cc.consumes();
      }

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
        edm::ParameterSetDescription desc;
        desc.add<edm::ESInputTag>("ModuleInfo",edm::ESInputTag(""));
        desc.add<int>("charMode",-1)->setComment("Manual override for characterization mode to unpack raw data");
        desc.add<int>("gain",-1)->setComment("Manual override for gain (1: 80 fC, 2: 160 fC, 4: 320 fC)");
        descriptions.addWithDefaultLabel(desc);
      }

      void setIndexFromModuleInfo(const HGCalCondSerializableModuleInfoRcd& iRecord) {
        const auto& moduleInfo = iRecord.get(moduleInfoToken_);
        cpi_.setMaxValues(moduleInfo);
      }

      std::optional<hgcalrechit::HGCalConfigParamHostCollection> produce(const HGCalCondSerializableConfigRcd& iRecord) {
        const auto& config = iRecord.get(configToken_);
        
        const uint32_t size =  cpi_.getSize(true); // ROC-level size
        hgcalrechit::HGCalConfigParamHostCollection product(size, cms::alpakatools::host());

        uint8_t gain = (uint8_t) (gain_>=1 ? gain_ : 1); // manual override
        product.view().config() = cpi_; // set dense indexing
        //uint32_t idx = cpi_.denseMap(id); // convert electronicsId to idx from denseMap
        product.view()[cpi_.denseROCMap(0*1024+0*64)].gain() = gain; // ROC 0, half 0
        product.view()[cpi_.denseROCMap(0*1024+1*64)].gain() = gain; // ROC 0, half 1
        product.view()[cpi_.denseROCMap(0*1024+2*64)].gain() = gain; // ROC 1, half 0
        product.view()[cpi_.denseROCMap(0*1024+3*64)].gain() = gain; // ROC 1, half 1
        product.view()[cpi_.denseROCMap(0*1024+4*64)].gain() = gain; // ROC 2, half 0
        product.view()[cpi_.denseROCMap(0*1024+5*64)].gain() = gain; // ROC 2, half 1
        product.view()[cpi_.denseROCMap(1*1024+0*64)].gain() = gain; // ROC 0, half 0
        product.view()[cpi_.denseROCMap(1*1024+1*64)].gain() = gain; // ROC 0, half 1
        product.view()[cpi_.denseROCMap(1*1024+2*64)].gain() = gain; // ROC 1, half 0
        product.view()[cpi_.denseROCMap(1*1024+3*64)].gain() = gain; // ROC 1, half 1
        product.view()[cpi_.denseROCMap(1*1024+4*64)].gain() = gain; // ROC 2, half 0
        product.view()[cpi_.denseROCMap(1*1024+5*64)].gain() = gain; // ROC 2, half 1

        //LogDebug("HGCalConfigurationESProducer") << "Placeholders: charMode=" << charMode_
        //  << ", gain=" << gain;
        std::cout << "HGCalConfigurationESProducer: Placeholders: charMode=" << charMode_
          << ", gain=" << (int) gain
          //<< "; YAML:" << config.moduleConfigs[0].gains[0]
          << std::endl;

        return product;
      }  // end of produce()

    private:
      edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
      edm::ESGetToken<HGCalCondSerializableConfig, HGCalCondSerializableConfigRcd> configToken_;
      HGCalCalibrationParameterIndex cpi_; // for dense indexing
      const int charMode_; // manual override of YAML files
      const int gain_; // manual override of YAML files

    };

  }  // namespace hgcalrechit

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(hgcalrechit::HGCalConfigurationESProducer);
