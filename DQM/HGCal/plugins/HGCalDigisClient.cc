#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"

#include <TString.h>

#include <string>
#include <fstream>
#include <iostream>

//#include "DQM/HGCal/interface/hgcalhit.h" // define DetectorId
#include "DQM/HGCal/interface/RunningCollection.h"
#include "DQM/HGCal/interface/LoadCalibrationParameters.h"

class HGCalDigisClient : public DQMEDAnalyzer {
public:
  explicit HGCalDigisClient(const edm::ParameterSet&);
  ~HGCalDigisClient() override;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  const edm::EDGetTokenT<HGCalElecDigiCollection> elecDigisToken_;
  const edm::EDGetTokenT<std::vector<int> > metadataToken_;

  MonitorElement* p_adc_minus_adcm1;
  MonitorElement* hex_adc_minus_adcm1;
  MonitorElement* hex_tot_median;
  MonitorElement* hex_beam_center;

  virtual void     export_calibration_parameters();

  // ------------ member data ------------
  std::string folder_;
  TString myTag;
  std::vector<int> calibration_flags;

  TString tag_calibration;
  TString tag_channelId;

  CalibrationParameterLoader calib_loader;
  RunningCollection myRunStatCollection;
  RunningStatistics myRecorder;
};


HGCalDigisClient::HGCalDigisClient(const edm::ParameterSet& iConfig)
  : elecDigisToken_(consumes<HGCalElecDigiCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
    metadataToken_(consumes<std::vector<int> >(iConfig.getParameter<edm::InputTag>("MetaData")))
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
    int trigtime(-999);
    if(metadata.size()>0) trigtime=metadata.at(0);
    LogDebug("HGCalDigisClient") << "trigtime=" << trigtime;
    
    //read digis
    const auto& elecDigis = iEvent.get(elecDigisToken_);
    for (auto& elecDigi : elecDigis) {
      
      bool isCM=elecDigi.id().isCM();
      int ch=elecDigi.id().rocChannel();
      
      LogDebug("HGCalDigisClient") << "Electronics Id= 0x" << std::hex << elecDigi.id().raw() << std::dec << std::endl
                                   << "adc = " << elecDigi.adc() << ", "
                                   << "adcm1 = " << elecDigi.adcm1() << ", "
                                   << "tot = " << elecDigi.tot() << ", "
                                   << "halfrocChannel = " << (uint32_t) elecDigi.id().halfrocChannel() << ", "
                                   << "channel = " << ch << ", CM=" << isCM;
      
      // Note: need logial mapping / electronics mapping
      uint16_t adc_diff = elecDigi.adc() - elecDigi.adcm1(); 
      p_adc_minus_adcm1->Fill( (uint32_t)elecDigi.id().halfrocChannel(), adc_diff);
      
      // Note: need CM info
      //myRunStatCollection.add_entry(globalChannelId_, adc_double_, adc_channel_CM);
    }
}

void HGCalDigisClient::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& iSetup) {
    ibook.setCurrentFolder("HGCAL/Digis");
    p_adc_minus_adcm1   = ibook.bookProfile("p_adc_minus_adcm1"   , ";channel;ADC #minux ADC_{-1}", 234 , 0 , 234 , 175 , -25 , 150 );

    ibook.setCurrentFolder("HGCAL/Maps");
    hex_adc_minus_adcm1 = ibook.book2DPoly ("hex_adc_minus_adcm1" , "hex_adc_minus_adcm1;x (arb. unit); y (arb. unit)" , -22 , 22 , -24 , 20);
    hex_tot_median      = ibook.book2DPoly ("hex_tot_median"      , "hex_tot_median;x (arb. unit); y (arb. unit)"      , -22 , 22 , -24 , 20);
    hex_beam_center     = ibook.book2DPoly ("hex_beam_center"     , "hex_beam_center;x (arb. unit); y (arb. unit)"     , -22 , 22 , -24 , 20);
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
        // Note: need CM info
        // bool isCM = ( channelId%39==37 || channelId%39==38 );
        HGCalElectronicsId id (1, 0, 0, 0, int(channelId/39), channelId%39);
        //HGCalElectronicsId id (0, 0, 0, int(channelId/39), channelId%39);
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
