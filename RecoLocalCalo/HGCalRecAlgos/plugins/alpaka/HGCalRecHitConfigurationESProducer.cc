#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
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


#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterIndex.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibrationParameterHostCollection.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/alpaka/HGCalCalibrationParameterDeviceCollection.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class HGCalRecHitConfigurationESProducer : public ESProducer {
  public:

    HGCalRecHitConfigurationESProducer(edm::ParameterSet const& iConfig)
      : ESProducer(iConfig) {

      auto cc = setWhatProduced(this);
      moduleInfoToken_ = cc.consumes();
    }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
      edm::ParameterSetDescription desc;
      //desc.add<std::string>("filename", {});
      desc.add<edm::ESInputTag>("ModuleInfo",edm::ESInputTag(""));
      descriptions.addWithDefaultLabel(desc);
    }

    std::optional<hgcalrechit::HGCalConfigParamHostCollection> produce(HGCalCondSerializableModuleInfoRcd const& iRecord) {
      auto const& moduleInfo = iRecord.get(moduleInfoToken_);

      // load config
      std::tuple<uint16_t,uint8_t,uint8_t,uint8_t> denseIdxMax = moduleInfo.getMaxValuesForDenseIndex();    

      HGCalCalibrationParameterIndex cpi;
      cpi.EventSLinkMax        = std::get<0>(denseIdxMax);
      cpi.sLinkCaptureBlockMax = std::get<1>(denseIdxMax);
      cpi.captureBlockECONDMax = std::get<2>(denseIdxMax);
      cpi.econdERXMax          = std::get<3>(denseIdxMax);

      uint32_t const size = cpi.EventSLinkMax*cpi.sLinkCaptureBlockMax*cpi.captureBlockECONDMax*cpi.econdERXMax;
      hgcalrechit::HGCalConfigParamHostCollection product(size, cms::alpakatools::host());


      uint8_t gain = 1; //(uint8_t) (gain_>=1 ? gain_ : 1); // manual override
      product.view().config() = cpi;
      //uint32_t idx = cpi.denseMap(id); // convert electronicsId to idx from denseMap 
      product.view()[cpi.denseMap(0*1024+0*64)].gain() = gain; // ROC 0, half 0
      product.view()[cpi.denseMap(0*1024+1*64)].gain() = gain; // ROC 0, half 1
      product.view()[cpi.denseMap(0*1024+2*64)].gain() = gain; // ROC 1, half 0
      product.view()[cpi.denseMap(0*1024+3*64)].gain() = gain; // ROC 1, half 1
      product.view()[cpi.denseMap(0*1024+4*64)].gain() = gain; // ROC 2, half 0
      product.view()[cpi.denseMap(0*1024+5*64)].gain() = gain; // ROC 2, half 1
      product.view()[cpi.denseMap(1*1024+0*64)].gain() = gain; // ROC 0, half 0
      product.view()[cpi.denseMap(1*1024+1*64)].gain() = gain; // ROC 0, half 1
      product.view()[cpi.denseMap(1*1024+2*64)].gain() = gain; // ROC 1, half 0
      product.view()[cpi.denseMap(1*1024+3*64)].gain() = gain; // ROC 1, half 1
      product.view()[cpi.denseMap(1*1024+4*64)].gain() = gain; // ROC 2, half 0
      product.view()[cpi.denseMap(1*1024+5*64)].gain() = gain; // ROC 2, half 1

      return product;
    } // end of produce()

  private:
    edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
    HGCalCondSerializableModuleInfo moduleInfo;
    //const std::string filename_;

  };

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_EVENTSETUP_ALPAKA_MODULE(HGCalRecHitConfigurationESProducer);
