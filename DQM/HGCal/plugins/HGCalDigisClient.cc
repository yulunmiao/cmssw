#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCalTestSystemMetadata.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiHostCollection.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <TString.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>

/**
   @short simple DQM client to monitor basic quantities at DIGI level
   @author Yu-Wei Kao
*/
class HGCalDigisClient : public DQMEDAnalyzer {
public:

  typedef HGCalCondSerializableModuleInfo::ModuleInfoKey_t MonitorKey_t;

  explicit HGCalDigisClient(const edm::ParameterSet&);
  ~HGCalDigisClient() override;

  /**
     @short method fills 'descriptions' with the allowed parameters for the module and their default values  
  */
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:

  /**
     @short takes care of booking the monitoring elements at the start of the run
     the histograms are instantiated per module according to the module mapping
     received from the event setup
   */
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;

  /**
     @short histogram filler per event
   */
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  
  const edm::EDGetTokenT<hgcaldigi::HGCalDigiHostCollection> digisToken_;
  const edm::EDGetTokenT<HGCalTestSystemMetaData> metadataToken_;

  std::map<MonitorKey_t, MonitorElement*> p_hitcount,p_maxadcvstrigtime,
    p_adc, p_tot, p_adcm, p_toa, p_maxadc, p_sums;
     
  // ------------ member data ------------
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  std::map<MonitorKey_t,MonitorKey_t> module_keys_;
};

//

HGCalDigisClient::HGCalDigisClient(const edm::ParameterSet& iConfig)
  : digisToken_(consumes<hgcaldigi::HGCalDigiHostCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
    metadataToken_(consumes<HGCalTestSystemMetaData>(iConfig.getParameter<edm::InputTag>("MetaData"))),
    moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd,edm::Transition::BeginRun>(iConfig.getParameter<edm::ESInputTag>("ModuleMapping")))
{}

//
HGCalDigisClient::~HGCalDigisClient() {
  LogDebug("HGCalDigisClient") << "End of the job" << std::endl;
}

//
void HGCalDigisClient::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  using namespace edm;
  
  //read digis and fill basic profiles
  const auto& digis = iEvent.getHandle(digisToken_);
  auto const& digis_view = digis->const_view();
  int32_t ndigis=digis_view.metadata().size();
  
  //start by computing the #fired TDC per e-Rx, average CM per e-Rx
  std::map<HGCalElectronicsId, float> nTDC,avgCM; 
  for(int32_t i = 0; i < ndigis; ++i) {

    auto digi = digis_view[i];
    HGCalElectronicsId eleid=HGCalElectronicsId(digi.electronicsId());
    HGCalElectronicsId erxid(eleid.zSide(),eleid.fedId(),eleid.captureBlock(),eleid.econdIdx(),eleid.econdeRx(),0);
    if(nTDC.count(erxid)==0) {
      nTDC[erxid]=0;
      avgCM[erxid]=0;
    }
    if(eleid.isCM()) {
      avgCM[erxid]+= 0.5*digi.adc();
    } else if( digi.tctp()>0 || digi.toa()>0) {
      nTDC[erxid] += 1;
    }
  }
  
  //fill the histograms / statistics
  std::map<MonitorKey_t, uint16_t> maxADC;
  for(int32_t i = 0; i < ndigis; ++i) {

    auto digi = digis_view[i];

    //identify the module key from the electronics id
    HGCalElectronicsId eleid=HGCalElectronicsId(digi.electronicsId());
    HGCalElectronicsId erxid(eleid.zSide(),eleid.fedId(),eleid.captureBlock(),eleid.econdIdx(),eleid.econdeRx(),0);
    bool isCM=eleid.isCM();
    int ch=eleid.halfrocChannel();
    int globalChannelId = 39*eleid.econdeRx() + ch;
    MonitorKey_t logiKey(eleid.zSide(), eleid.fedId(),eleid.captureBlock(), eleid.econdIdx());
    MonitorKey_t geomKey = module_keys_[logiKey];
    LogDebug("HGCalDigisClient") << "Electronics Id= 0x" << std::hex << eleid.raw() << std::dec << std::endl
                                 << "adc = "             << digi.adc()                              << ", "
                                 << "adcm1 = "           << digi.adcm1()                            << ", "
                                 << "tot = "             << digi.tot()                              << ", "
                                 << "halfrocChannel = "  << (uint32_t) eleid.halfrocChannel()   << ", "
                                 << "channel = "         << globalChannelId
                                 << ", CM=" << isCM;

    // fill monitoring elements for digis
    uint8_t tctp        = digi.tctp();
    uint16_t adc        = digi.adc();
    uint16_t tot        = digi.tot();
    uint16_t toa        = digi.toa();
    uint16_t adcm       = digi.adcm1();

    if(adc>0 || tot>0) p_hitcount[geomKey]->Fill(globalChannelId);

    p_adcm[geomKey]->Fill(globalChannelId, adcm);
    p_toa[geomKey]->Fill(globalChannelId, toa);

      
    //TOT
    if(tctp==3) {
      p_tot[geomKey]->Fill(globalChannelId, tot);
    }
    
    //ADC
    else {
      p_adc[geomKey]->Fill(globalChannelId, adc);
      if(maxADC.count(geomKey)==0) maxADC[geomKey]=0;
      maxADC[geomKey]=std::max(maxADC[geomKey],adc);
      
      //increment sums of this channel
      float cm=avgCM[erxid];
      float tdc=nTDC[erxid];
      std::vector<double> tosum = {1,
                                   (double)adc,pow(adc,2),
                                   cm,pow(cm,2),adc*cm,
                                   (double)adcm,pow(adcm,2),(double)adc*adcm,
                                   tdc,pow(tdc,2),adc*tdc};
      for(size_t k=1; k<=tosum.size(); k++) 
        p_sums[geomKey]->getBinContent(globalChannelId+k,k,p_sums[geomKey]->getBinContent(globalChannelId+k,k)+tosum[k-1]);
    }
  }

  //read trigtime and fill monitoring elements for the leading hit
  const auto& metadata = iEvent.get(metadataToken_);
  int trigTime = metadata.trigTime_;
  LogDebug("HGCalDigisClient") << "trigTime=" << trigTime;
  for(auto it : maxADC) {
    p_maxadcvstrigtime[it.first]->Fill(trigTime, it.second);
    p_maxadc[it.first]->Fill(it.second);
  }
}

//
void HGCalDigisClient::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {

  //create module keys
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  module_keys_ = moduleInfo.getAsSimplifiedModuleLocatorMap(true);
  LogDebug("HGCalDigisClient") << "Read module info with " << module_keys_.size() << " entries";

  //book monitoring elements (histos, profiles, etc.)
  ibook.setCurrentFolder("HGCAL/Digis");
  for(auto m : moduleInfo.params_) {
    TString tag=Form("%d_%d_%d_%d",m.zside,m.plane,m.u,m.v);
    MonitorKey_t k(m.zside,m.plane,m.u,m.v);
    int nch(39*6*(1+m.isHD));
    p_hitcount[k] = ibook.book1D("hitcount_"+tag,           ";Channel; #hits",   nch, 0, nch);
    p_maxadcvstrigtime[k] = ibook.book2D("maxadc_vs_trigtime_"+tag, ";max ADC; Counts",  100, 0, 100, 100, -0.5, 99.5);   
    p_sums[k]             = ibook.book2D("sums_"+tag, ";Channel;", nch,0,nch, 12,0,12);
    p_sums[k]->setBinLabel(1,"N",2);
    p_sums[k]->setBinLabel(2,"#sum ADC",2);
    p_sums[k]->setBinLabel(3,"#sum ADC^{2}",2);
    p_sums[k]->setBinLabel(4,"#sum CM",2);
    p_sums[k]->setBinLabel(5,"#sum CM^{2}",2);
    p_sums[k]->setBinLabel(6,"#sum ADC*CM",2);
    p_sums[k]->setBinLabel(7,"#sum ADC_{-1}",2);
    p_sums[k]->setBinLabel(8,"#sum ADC_{-1}^{2}",2);
    p_sums[k]->setBinLabel(9,"#sum ADC*ADC_{-1}",2);
    p_sums[k]->setBinLabel(10,"#sum TDC",2);
    p_sums[k]->setBinLabel(11,"#sum TDC^{2}",2);
    p_sums[k]->setBinLabel(12,"#sum ADC*TDC",2);
    p_maxadc[k]           = ibook.book1D("maxadc_"+tag,             ";max ADC; Counts",  100, -0.5, 99.5); 
    p_adc[k]              = ibook.bookProfile("p_adc_" + tag,       ";Channel; ADC",     nch, 0, nch, 150, 0, 150);
    p_adc[k]->setOption("s"); //save standard deviation instead of error mean for noise estimate
    p_tot[k]              = ibook.bookProfile("p_tot_" + tag,       ";Channel; TOT",     nch, 0, nch, 150, 0, 150);
    p_adcm[k]             = ibook.bookProfile("p_adcm_"+ tag,       ";Channel; ADC(-1)", nch, 0, nch, 150, 0, 150);
    p_toa[k]              = ibook.bookProfile("p_toa_" + tag,       ";Channel; TOA",     nch, 0, nch, 150, 0, 150);
  }
}

//
void HGCalDigisClient::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("Digis",edm::InputTag("hgcalDigis","DIGI"));
  desc.add<edm::InputTag>("MetaData",edm::InputTag("hgcalEmulatedSlinkRawData","hgcalMetaData"));
  desc.add<edm::ESInputTag>("ModuleMapping",edm::ESInputTag(""));
}

// define this as a plug-in
DEFINE_FWK_MODULE(HGCalDigisClient);
