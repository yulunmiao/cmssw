#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "EventFilter/HGCalRawToDigi/interface/HGCalUnpacker.h"

#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHostCollection.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableConfigRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableConfig.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"



class HGCalRawToDigi : public edm::stream::EDProducer<> {
public:
  explicit HGCalRawToDigi(const edm::ParameterSet&);

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  const edm::EDGetTokenT<FEDRawDataCollection> fedRawToken_;
  const edm::EDPutTokenT<HGCalElecDigiCollection> elecDigisToken_;
  const edm::EDPutTokenT<HGCalElecDigiCollection> elecCMsToken_;
  const edm::EDPutTokenT<hgcaldigi::HGCalDigiHostCollection> elecDigisSoAToken_;
  edm::ESWatcher<HGCalCondSerializableConfigRcd> configWatcher_;
  edm::ESGetToken<HGCalCondSerializableConfig,HGCalCondSerializableConfigRcd> configToken_;
  edm::ESWatcher<HGCalCondSerializableModuleInfoRcd> eleMapWatcher_;
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  HGCalCondSerializableModuleInfo::ERxBitPatternMap erxEnableBits_;
  
  const std::vector<unsigned int> fedIds_;
  const unsigned int badECONDMax_;
  const unsigned int numERxsInECOND_;
  const std::unique_ptr<HGCalUnpacker> unpacker_;
  HGCalModuleConfig moduleConfig_; // current module

};

HGCalRawToDigi::HGCalRawToDigi(const edm::ParameterSet& iConfig)
    : fedRawToken_(consumes<FEDRawDataCollection>(iConfig.getParameter<edm::InputTag>("src"))),
      elecDigisToken_(produces<HGCalElecDigiCollection>("DIGI")),
      elecCMsToken_(produces<HGCalElecDigiCollection>("CM")),
      elecDigisSoAToken_(produces<hgcaldigi::HGCalDigiHostCollection>()),
      configToken_(esConsumes<HGCalCondSerializableConfig,HGCalCondSerializableConfigRcd>(
                         iConfig.getParameter<edm::ESInputTag>("config_label"))),
      moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd>(
                         iConfig.getParameter<edm::ESInputTag>("module_info_label"))),
      fedIds_(iConfig.getParameter<std::vector<unsigned int> >("fedIds")),
      badECONDMax_(iConfig.getParameter<unsigned int>("badECONDMax")),
      numERxsInECOND_(iConfig.getParameter<unsigned int>("numERxsInECOND")),
      unpacker_(new HGCalUnpacker(
          HGCalUnpackerConfig{.sLinkBOE = iConfig.getParameter<unsigned int>("slinkBOE"),
                              .captureBlockReserved = iConfig.getParameter<unsigned int>("captureBlockReserved"),
                              .econdHeaderMarker = iConfig.getParameter<unsigned int>("econdHeaderMarker"),
                              .sLinkCaptureBlockMax = iConfig.getParameter<unsigned int>("maxCaptureBlock"),
                              .captureBlockECONDMax = iConfig.getParameter<unsigned int>("captureBlockECONDMax"),
                              .econdERXMax = iConfig.getParameter<unsigned int>("econdERXMax"),
                              .erxChannelMax = iConfig.getParameter<unsigned int>("erxChannelMax"),
                              .payloadLengthMax = iConfig.getParameter<unsigned int>("payloadLengthMax"),
                              .channelMax = iConfig.getParameter<unsigned int>("channelMax"),
                              .commonModeMax = iConfig.getParameter<unsigned int>("commonModeMax")})) {}

//
void HGCalRawToDigi::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  // retrieve the FED raw data
  const auto& raw_data = iEvent.get(fedRawToken_);

  // retrieve module info
  if(eleMapWatcher_.check(iSetup)){
    //read default enabled e-Rx bits from module locator
    auto moduleInfo = iSetup.getData(moduleInfoToken_);
    erxEnableBits_ = moduleInfo.getERxBitPattern();
    std::cout << "eRx enabled bits" << std::endl;
    for(auto it : erxEnableBits_)
      edm::LogInfo("HGCalRawToDigi") << "(" << std::get<0>(it.first) << "," << std::get<1>(it.first) << "," << std::get<2>(it.first) << ") = "
                                     << "0x" << std::hex << it.second << std::dec;
  }
  
  
  // retrieve configuration from YAML files
  if (configWatcher_.check(iSetup)) {
    auto conds = iSetup.getData(configToken_);
    size_t nmods = conds.moduleConfigs.size();
    edm::LogInfo("HGCalRawToDigi") << "Conditions retrieved for " << nmods << " modules:\n" << conds;
    for (auto it : conds.moduleConfigs) { // loop over map module electronicsId -> HGCalModuleConfig
      HGCalModuleConfig moduleConfig(it.second);
      edm::LogInfo("HGCalRawToDigi")
        << "  Module " << it.first << ": charMode=" << moduleConfig.charMode;
    }
    moduleConfig_ = conds.moduleConfigs[0]; // for now use module with electronicsId = 0 as placeholder
  } // else: use previously loaded module configuration

  // prepare the output
  HGCalDigiCollection digis;
  HGCalElecDigiCollection elec_digis;
  HGCalElecDigiCollection elec_cms;
  std::vector<uint32_t> elecid;
  std::vector<uint8_t> tctp;
  std::vector<uint16_t> adcm1;
  std::vector<uint16_t> adc;
  std::vector<uint16_t> tot;
  std::vector<uint16_t> toa;
  std::vector<uint16_t> cm; 
  for (const auto& fed_id : fedIds_) {
    const auto& fed_data = raw_data.FEDData(fed_id);
    if (fed_data.size() == 0)
      continue;

    std::vector<uint32_t> data_32bit;
    auto* ptr = fed_data.data();
    size_t fed_size = fed_data.size();
    for (size_t i = 0; i < fed_size; i += 4){
      data_32bit.emplace_back(((*(ptr + i) & 0xff) << 0) + (((i + 1) < fed_size) ? ((*(ptr + i + 1) & 0xff) << 8) : 0) +
                              (((i + 2) < fed_size) ? ((*(ptr + i + 2) & 0xff) << 16) : 0) +
                              (((i + 3) < fed_size) ? ((*(ptr + i + 3) & 0xff) << 24) : 0));
    }

    unpacker_->parseSLink(
        data_32bit,
        [this](uint16_t /*sLink*/, uint8_t /*captureBlock*/, uint8_t /*econd*/) { return (1 << numERxsInECOND_) - 1; });

    auto channeldata = unpacker_->channelData();
    auto commonModeSum=unpacker_->commonModeSum();
    for (unsigned int i = 0; i < channeldata.size(); i++) {
      auto data = channeldata.at(i);
      const auto& id = data.id();
      auto idraw = id.raw();
      auto raw = data.raw();
      LogDebug("HGCalRawToDigi:produce") << "channel data, id=" << idraw << ", raw=" << raw;
      elec_digis.push_back(data);
      elecid.push_back(id.raw());
      tctp.push_back(data.tctp());
      adcm1.push_back(data.adcm1(moduleConfig_.charMode));
      adc.push_back(data.adc(moduleConfig_.charMode));
      tot.push_back(data.tot(moduleConfig_.charMode));
      toa.push_back(data.toa());
      cm.push_back(commonModeSum.at(i));
    }

    auto commonmode = unpacker_->commonModeData();
    for (unsigned int i = 0; i < commonmode.size(); i++) {
      auto cm = commonmode.at(i);
      const auto& id = cm.id();
      auto idraw = id.raw();
      auto raw = cm.raw();
      LogDebug("HGCalRawToDigi:produce") << "common modes, id=" << idraw << ", raw=" << raw;
      elec_cms.push_back(cm);
    }

    if (const auto& bad_econds = unpacker_->badECOND(); !bad_econds.empty()) {
      if (bad_econds.size() > badECONDMax_)
        throw cms::Exception("HGCalRawToDigi:produce")
            << "Too many bad ECON-Ds: " << bad_econds.size() << " > " << badECONDMax_ << ".";
      edm::LogWarning("HGCalRawToDigi:produce").log([&bad_econds](auto& log) {
        log << "Bad ECON-D: " << std::dec;
        std::string prefix;
        for (const auto& badECOND : bad_econds)
          log << prefix << badECOND, prefix = ", ";
        log << ".";
      });

    }
  }

  //auto elec_digis_soa = std::make_unique<hgcaldigi::HGCalDigiHostCollection>(elec_digis.size(), cms::alpakatools::host());
  hgcaldigi::HGCalDigiHostCollection elec_digis_soa(elec_digis.size(),cms::alpakatools::host());
  for (unsigned int i = 0; i < elecid.size(); i++) {
      elec_digis_soa.view()[i].electronicsId() = elecid.at(i);
      elec_digis_soa.view()[i].tctp() = tctp.at(i);
      elec_digis_soa.view()[i].adcm1() = adcm1.at(i);
      elec_digis_soa.view()[i].adc() = adc.at(i);
      elec_digis_soa.view()[i].tot() = tot.at(i);
      elec_digis_soa.view()[i].toa() = toa.at(i);
      elec_digis_soa.view()[i].cm() = cm.at(i);
      elec_digis_soa.view()[i].flags() = 0;
  }

  iEvent.emplace(elecDigisToken_, std::move(elec_digis));
  iEvent.emplace(elecCMsToken_, std::move(elec_cms));
  iEvent.emplace(elecDigisSoAToken_, std::move(elec_digis_soa));
}

//
void HGCalRawToDigi::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src", edm::InputTag("rawDataCollector"));
  desc.add<unsigned int>("maxCaptureBlock", 1)->setComment("maximum number of capture blocks in one S-Link");
  desc.add<unsigned int>("captureBlockReserved", 0)->setComment("capture block reserved pattern");
  desc.add<unsigned int>("econdHeaderMarker", 0x154)->setComment("ECON-D header Marker pattern");
  desc.add<unsigned int>("slinkBOE", 0x2a)->setComment("SLink BOE pattern");
  desc.add<unsigned int>("captureBlockECONDMax", 12)->setComment("maximum number of ECON-D's in one capture block");
  desc.add<unsigned int>("econdERXMax", 12)->setComment("maximum number of eRx's in one ECON-D");
  desc.add<unsigned int>("erxChannelMax", 37)->setComment("maximum number of channels in one eRx");
  desc.add<unsigned int>("payloadLengthMax", 469)->setComment("maximum length of payload length");
  desc.add<unsigned int>("channelMax", 7000000)->setComment("maximum number of channels unpacked");
  desc.add<unsigned int>("commonModeMax", 4000000)->setComment("maximum number of common modes unpacked");
  desc.add<unsigned int>("badECONDMax", 200)->setComment("maximum number of bad ECON-D's");
  desc.add<std::vector<unsigned int> >("fedIds", {});
  desc.add<unsigned int>("numERxsInECOND", 12)->setComment("number of eRxs in each ECON-D payload");
  desc.add<edm::ESInputTag>("config_label", edm::ESInputTag(""))->setComment("label for HGCalConfigESSourceFromYAML reader");
  desc.add<edm::ESInputTag>("module_info_label", edm::ESInputTag(""))->setComment("label for HGCalModuleInfoESSource");
  descriptions.add("hgcalDigis", desc);
}

// define this as a plug-in
DEFINE_FWK_MODULE(HGCalRawToDigi);
