#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCalTestSystemMetadata.h"
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <TString.h>
#include <TFile.h>
#include <TKey.h>

#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <tuple>

//#include "DQM/HGCal/interface/hgcalhit.h" // define DetectorId
#include "DQM/HGCal/interface/RunningCollection.h"
#include "DQM/HGCal/interface/LoadCalibrationParameters.h"

class HGCalDigisClient : public DQMEDAnalyzer {
public:

  typedef HGCalCondSerializableModuleInfo::ModuleInfoKey_t MonitorKey_t;
  
  explicit HGCalDigisClient(const edm::ParameterSet&);
  ~HGCalDigisClient() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  const edm::EDGetTokenT<HGCalElecDigiCollection> elecDigisToken_;
  const edm::EDGetTokenT<HGCalTestSystemMetaData> metadataToken_;

  std::map<MonitorKey_t, MonitorElement*> p_adc;
  std::map<MonitorKey_t, MonitorElement*> p_adcm;
  std::map<MonitorKey_t, MonitorElement*> p_adc_minus_adcm1;
  std::map<MonitorKey_t, MonitorElement*> hex_channelId;
  std::map<MonitorKey_t, MonitorElement*> hex_adc_minus_adcm;
  std::map<MonitorKey_t, MonitorElement*> hex_tot_mean;
  std::map<MonitorKey_t, MonitorElement*> hex_beam_center;

  int hex_counter;

  virtual void export_calibration_parameters();

  // ------------ member data ------------
  std::string folder_;
  TString myTag;
  std::vector<int> calibration_flags;

  TString tag_calibration;
  TString tag_channelId;

  CalibrationParameterLoader calib_loader;
  RunningCollection myRunStatCollection;
  RunningStatistics myRecorder;
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  std::map<MonitorKey_t,MonitorKey_t> module_keys_;
};


HGCalDigisClient::HGCalDigisClient(const edm::ParameterSet& iConfig)
  : elecDigisToken_(consumes<HGCalElecDigiCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
    metadataToken_(consumes<HGCalTestSystemMetaData>(iConfig.getParameter<edm::InputTag>("MetaData"))),
    moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd,edm::Transition::BeginRun>(edm::ESInputTag(iConfig.getParameter<std::string>("ModuleMapping"))))
{}

HGCalDigisClient::~HGCalDigisClient() {
    // TODO: is the destructor a proper place to export calibration parameters?
    // export_calibration_parameters();
  LogDebug("HGCalDigisClient") << "End of the job" << std::endl;
}

// ------------ method called for each event  ------------
void HGCalDigisClient::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    using namespace edm;

    //read trigtime
    const auto& metadata = iEvent.get(metadataToken_);
    int trigTime = metadata.trigTime_;
    LogDebug("HGCalDigisClient") << "trigTime=" << trigTime;
    
    //read digis
    const auto& elecDigis = iEvent.get(elecDigisToken_);
    for (auto& elecDigi : elecDigis) {

      MonitorKey_t logiKey(elecDigi.id().zSide(),
                           elecDigi.id().fedId(),
                           elecDigi.id().captureBlock(),
                           elecDigi.id().econdIdx());
      MonitorKey_t geomKey = module_keys_[logiKey];
      
      bool isCM=elecDigi.id().isCM();
      int ch=elecDigi.id().rocChannel();
      LogDebug("HGCalDigisClient") << "Electronics Id= 0x" << std::hex << elecDigi.id().raw() << std::dec << std::endl
                                   << "adc = " << elecDigi.adc() << ", "
                                   << "adcm1 = " << elecDigi.adcm1() << ", "
                                   << "tot = " << elecDigi.tot() << ", "
                                   << "halfrocChannel = " << (uint32_t) elecDigi.id().halfrocChannel() << ", "
                                   << "channel = " << ch << ", CM=" << isCM;
      
      // Note: need logial mapping / electronics mapping
      HGCalElectronicsId id = elecDigi.id();
      uint16_t adc      = elecDigi.adc();
      uint16_t adcm     = elecDigi.adcm1();
      uint16_t adc_diff = elecDigi.adc() - elecDigi.adcm1();
      int globalChannelId = 39*id.econdeRx() + id.halfrocChannel();
      p_adc[geomKey]             -> Fill( globalChannelId, adc      );
      p_adcm[geomKey]            -> Fill( globalChannelId, adcm     );
      p_adc_minus_adcm1[geomKey] -> Fill( globalChannelId, adc_diff );

      // Note: need CM info
      //myRunStatCollection.add_entry(globalChannelId_, adc_double_, adc_channel_CM);
    }
}

void HGCalDigisClient::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {

  //create module keys
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  module_keys_ = moduleInfo.getAsSimplifiedModuleLocatorMap(true);
  LogDebug("HGCalDigisClient") << "Read module info with " << module_keys_.size() << " entries";

  ibook.setCurrentFolder("HGCAL/Digis");
  for(auto kit : module_keys_) {
    MonitorKey_t k(kit.second);
    TString tag=Form("%d_%d_%d_%d",std::get<0>(k),std::get<1>(k),std::get<2>(k),std::get<3>(k));
    p_adc[k]               = ibook.bookProfile("p_adc"              + tag , ";channel;ADC #minux ADC_{-1}" , 234 , 0 , 234 , 175 , -25 , 150 );
    p_adcm[k]              = ibook.bookProfile("p_adcm"             + tag , ";channel;ADC #minux ADC_{-1}" , 234 , 0 , 234 , 175 , -25 , 150 );
    p_adc_minus_adcm1[k]   = ibook.bookProfile("p_adc_minus_adcm1_" + tag , ";channel;ADC #minux ADC_{-1}" , 234 , 0 , 234 , 175 , -25 , 150 );
  }

  ibook.setCurrentFolder("HGCAL/Maps");
  for(auto kit : module_keys_) {
    MonitorKey_t k(kit.second);
    TString tag=Form("%d_%d_%d_%d",std::get<0>(k),std::get<1>(k),std::get<2>(k),std::get<3>(k));
    TString xytitle = ";x (cm);y (cm)";
    hex_channelId[k]      = ibook.book2DPoly("hex_channelId"      + tag, "hex_channelId"      + xytitle , -26 , 26 , -28 , 24);
    hex_adc_minus_adcm[k] = ibook.book2DPoly("hex_adc_minus_adcm" + tag, "hex_adc_minus_adcm" + xytitle , -26 , 26 , -28 , 24);
    hex_tot_mean[k]       = ibook.book2DPoly("hex_tot_mean"       + tag, "hex_tot_mean"       + xytitle , -26 , 26 , -28 , 24);
    hex_beam_center[k]    = ibook.book2DPoly("hex_beam_center"    + tag, "hex_beam_center"    + xytitle , -26 , 26 , -28 , 24);
  }
  
    //--------------------------------------------------
    // load geometry
    //--------------------------------------------------
    // Note: creating same wafer map for each polygonal monitor element for TB2023 in August
    TString root_geometry = "/afs/cern.ch/work/y/ykao/public/raw_data_handling/hexagons_20230626.root";
    TFile *fgeo = new TFile(root_geometry, "R");

    TGraph *gr;
    TKey *key;
    TIter nextkey(fgeo->GetDirectory(nullptr)->GetListOfKeys());
    for(auto kit : module_keys_) {
      MonitorKey_t k(kit.second);

      hex_counter = 0;
      while ((key = (TKey*)nextkey())) {
          TObject *obj = key->ReadObj();
          if(obj->InheritsFrom("TGraph")) {
              gr = (TGraph*) obj;
              hex_channelId[k]      -> addBin(gr);
              hex_adc_minus_adcm[k] -> addBin(gr);
              hex_tot_mean[k]       -> addBin(gr);
              hex_beam_center[k]    -> addBin(gr);
              hex_counter+=1;
          }
      }

      // fill info for channel Id
      for(int i=0; i<hex_counter; ++i) {
          if(i==0)
              hex_channelId[k]->setBinContent(i+1, 1e-6);
          else
              hex_channelId[k]->setBinContent(i+1, i);
      }
    } // end of module keys

    fgeo->Close();
}

void HGCalDigisClient::export_calibration_parameters() {
    //TString csv_file_name = "./meta_conditions/output_DQMEDAnalyzer_calibration_parameters_" + myTag + "Data" + tag_calibration + ".txt";
    TString csv_file_name = "./meta_conditions/output_DQMEDAnalyzer_calibration_parameters.txt";
    std::ofstream myfile(csv_file_name.Data());

    std::vector<RunningStatistics> mRs = myRunStatCollection.get_vector_running_statistics();

    myfile << "Channel Pedestal CM_slope CM_offset kappa_BXm1\n";
    for(int channelId=0; channelId<234; ++channelId) {
        double kappa_BXm1 = 0.000;
        RunningStatistics rs = mRs[channelId];
        HGCalElectronicsId id (1, 0, 0, 0, int(channelId/39), channelId%39); // zside, fedid, captureblock, econdidx, econderx, halfrocch
        myfile << Form("%d %f %f %f %f\n", id.raw(), rs.get_mean_adc(), rs.get_slope(), rs.get_intercept(), kappa_BXm1);
    }
    myfile.close();
    LogDebug("HGCalDigisClient") << "Export CM parameters @ " << csv_file_name << std::endl;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HGCalDigisClient::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("Digis",edm::InputTag("hgcalDigis"));
    desc.add<edm::InputTag>("MetaData",edm::InputTag("hgcalMetaData"));
}

// define this as a plug-in
DEFINE_FWK_MODULE(HGCalDigisClient);
