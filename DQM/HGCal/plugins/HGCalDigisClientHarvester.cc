#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <string>
#include <sstream>
#include <math.h>

//Framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/ESWatcher.h"

//DQM
#include "DQMServices/Core/interface/DQMEDHarvester.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableModuleInfoRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <TString.h>
#include <TFile.h>
#include <TKey.h>

#include <fstream>
#include <iostream>

/**
   @short DQM harvester for the DIGI monitoring elements at the end of lumi section / run 
*/
class HGCalDigisClientHarvester: public DQMEDHarvester{

public:

  typedef HGCalCondSerializableModuleInfo::ModuleInfoKey_t MonitorKey_t;
  
  HGCalDigisClientHarvester(const edm::ParameterSet& ps);
  virtual ~HGCalDigisClientHarvester();
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

protected:
  
  void beginJob();
  void dqmEndLuminosityBlock(DQMStore::IBooker &, DQMStore::IGetter &, edm::LuminosityBlock const &, edm::EventSetup const&);
  void dqmEndJob(DQMStore::IBooker &, DQMStore::IGetter &) override; 

private:

  /**
     @short saves the level-0 calibration parameters from the pedestal profiles of different modules harvested
   */
  void export_calibration_parameters(std::map<MonitorKey_t,const MonitorElement*> &pedestals);

  //location of the hex map templates
  std::string templateROOT_;

  //monitoring elements
  std::map<MonitorKey_t, MonitorElement*> hex_channelId, hex_occupancy;

  //module mapper stuff
  edm::ESGetToken<HGCalCondSerializableModuleInfo, HGCalCondSerializableModuleInfoRcd> moduleInfoToken_;
  edm::ESWatcher<HGCalCondSerializableModuleInfoRcd> miWatcher_;
  std::map<MonitorKey_t,MonitorKey_t> module_keys_;

  //output file with pedestals etc.
  std::string level0CalibOut_;
};

//
HGCalDigisClientHarvester::HGCalDigisClientHarvester(const edm::ParameterSet& iConfig)
  : templateROOT_(iConfig.getParameter<std::string>("HexTemplateFile")),
    moduleInfoToken_(esConsumes<HGCalCondSerializableModuleInfo,HGCalCondSerializableModuleInfoRcd,edm::Transition::EndLuminosityBlock>(iConfig.getParameter<edm::ESInputTag>("ModuleMapping"))),
    level0CalibOut_(iConfig.getParameter<std::string>("Level0CalibOut"))
{
}

//
void HGCalDigisClientHarvester::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::ESInputTag>("ModuleMapping",edm::ESInputTag(""));
  desc.add<std::string>("HexTemplateFile","/afs/cern.ch/work/y/ykao/public/raw_data_handling/hexagons_20230626.root");
  desc.add<std::string>("Level0CalibOut","level0_calibration_parameters.txt");
}


//
HGCalDigisClientHarvester::~HGCalDigisClientHarvester()
{
  edm::LogInfo("HGCalDigisClientHarvester") <<  "@ DTOR";
}

//
void HGCalDigisClientHarvester::beginJob()
{
  edm::LogInfo("HGCalDigisClientHarvester") << " @ beginJob";
}

//
void HGCalDigisClientHarvester::dqmEndLuminosityBlock(DQMStore::IBooker & ibooker, DQMStore::IGetter & igetter, edm::LuminosityBlock const & iLumi, edm::EventSetup const& iSetup) 
{

  //book histos only if module info changed (should happen end of first lumi section)
  if (!miWatcher_.check(iSetup)) return;
  
  //read module mapper and build list of module tags to retrieve the monitoring elemnts for
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  module_keys_ = moduleInfo.getAsSimplifiedModuleLocatorMap(true);
  for(auto it: module_keys_) {

    MonitorKey_t k(it.second);
    TString tag=Form("%d_%d_%d_%d",std::get<0>(k),std::get<1>(k),std::get<2>(k),std::get<3>(k));

    ibooker.setCurrentFolder("HGCAL/Maps");    
    hex_channelId[k] = ibooker.book2DPoly("hex_channelId" + tag, "; x[cm]; y[cm];ID", -26 , 26 , -28 , 24);
    hex_occupancy[k] = ibooker.book2DPoly("hex_occupancy" + tag, "; x[cm]; y[cm];Occupancy", -26 , 26 , -28 , 24);
  }

  //fill the 2DPoly from the hexmap external templates
  TFile *fgeo = new TFile(templateROOT_.c_str(), "R");
  TGraph *gr;
  TKey *key;
  TIter nextkey(fgeo->GetDirectory(nullptr)->GetListOfKeys());
  int i=0;
  while ((key = (TKey*)nextkey())) {
    TObject *obj = key->ReadObj();
    if(!obj->InheritsFrom("TGraph")) continue;
    gr = (TGraph*) obj;
    i++;    
    for(auto kit : hex_channelId) {
      MonitorKey_t k(kit.first);
      hex_channelId[k]->addBin(gr);
      hex_channelId[k]->setBinContent(i+1, i==0 ? 1e-6 : i); 
      hex_occupancy[k]->addBin(gr);
    }
  }
  fgeo->Close();

  edm::LogInfo("HGCalDigisClientHarvester") << "Retrieved : " << module_keys_.size() << " module tags to harvest and defined hexmaps";
}

//
void HGCalDigisClientHarvester::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{

  //loop over modules to harvest
  std::map<MonitorKey_t,const MonitorElement*> pedestals;
  for(auto it : module_keys_) {
    
    MonitorKey_t k(it.second);
    TString tag=Form("%d_%d_%d_%d",std::get<0>(k),std::get<1>(k),std::get<2>(k),std::get<3>(k));

    std::string meName("HGCAL/Digis/p_adc_"+tag);
    const MonitorElement* numerator = igetter.get(meName);
    if (numerator == nullptr) continue;

    pedestals[it.first]=numerator;
    std::cout << " retrieved ADC profile for " << tag << std::endl;
  }

  //save pedestals to file
  export_calibration_parameters(pedestals);
}


//
void HGCalDigisClientHarvester::export_calibration_parameters(std::map<MonitorKey_t,const MonitorElement*> &pedestals) {

  //open txt file
  std::ofstream myfile(level0CalibOut_);

  myfile << "Channel Pedestal CM_slope CM_offset kappa_BXm1\n";
  for(auto it : pedestals) {

    MonitorKey_t k(it.first);
    bool zside = std::get<0>(k);
    uint16_t slink = std::get<1>(k);
    uint16_t captureblock = std::get<2>(k);
    uint16_t econd = std::get<3>(k);

    const MonitorElement *me=it.second;
    for(int ibin=0; ibin<me->getNbinsX(); ibin++) {
      uint16_t erx = ibin/39;
      uint16_t seq = ibin%39;
      HGCalElectronicsId id(zside,slink,captureblock,econd,erx,seq);

      myfile << "0x" << std::hex << id.raw() << " " << std::dec
             << std::setprecision(3)
             << me->getBinContent(ibin+1) << " " //<< me->getBinError(ibin+1) << " "
             << 0.f << " "
             << 0.f << " "
             << 0.f << std::endl;
    }
    
  }

  myfile.close();

  edm::LogInfo("HGCalDigisClient") << "Export CM parameters @ " << level0CalibOut_ << std::endl;
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HGCalDigisClientHarvester);
