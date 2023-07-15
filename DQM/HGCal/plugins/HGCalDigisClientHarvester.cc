#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQM/HGCal/interface/CellStatistics.h"

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
  void export_calibration_parameters(std::map<HGCalElectronicsId,hgcal::CellStatistics> &);

  //location of the hex map templates
  std::string templateROOT_;

  //monitoring elements
  std::map<MonitorKey_t, MonitorElement*> //hex_channelId, 
    hex_pedestal,hex_noise, hex_cmrho, hex_bxm1rho,
    p_coeffs;

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
  desc.add<std::string>("HexTemplateFile","/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/hexagons_20230626.root");
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
  
  //read module mapper and build list of module tags to retrieve the monitoring elements for
  auto moduleInfo = iSetup.getData(moduleInfoToken_);
  module_keys_ = moduleInfo.getAsSimplifiedModuleLocatorMap(true);
  for(auto m : moduleInfo.params_) {

    TString tag=Form("%d_%d_%d_%d",m.zside,m.plane,m.u,m.v);
    MonitorKey_t k(m.zside,m.plane,m.u,m.v);
    int nch(39*6*(1+m.isHD));

    ibooker.setCurrentFolder("HGCAL/Summary");    
    //hex_channelId[k] = ibooker.book2DPoly("hex_channelId" + tag, "; x[cm]; y[cm];ID", -26 , 26 , -28 , 24);
    hex_pedestal[k]  = ibooker.book2DPoly("hex_pedestal" + tag,  "; x[cm]; y[cm];Pedestal", -26 , 26 , -28 , 24);
    hex_noise[k]     = ibooker.book2DPoly("hex_noise" + tag,     "; x[cm]; y[cm];Noise", -26 , 26 , -28 , 24);
    hex_cmrho[k]     = ibooker.book2DPoly("hex_cmrho" + tag,     "; x[cm]; y[cm];#rho(CM)", -26 , 26 , -28 , 24);
    hex_bxm1rho[k]   = ibooker.book2DPoly("hex_bxm1rho" + tag,   "; x[cm]; y[cm];#rho(ADC_{-1})", -26 , 26 , -28 , 24);

    p_coeffs[k] = ibooker.book2D("coeffs_"+tag, ";Channel;", nch,0,nch, 11,0,11);
    p_coeffs[k]->setBinLabel(1,"<ADC>",2);
    p_coeffs[k]->setBinLabel(2,"#sigma(ADC)",2);
    p_coeffs[k]->setBinLabel(3,"#rho(ADC,CM)",2);
    p_coeffs[k]->setBinLabel(4,"k(ADC,CM)",2);
    p_coeffs[k]->setBinLabel(5,"o(ADC,CM)",2);
    p_coeffs[k]->setBinLabel(6,"#rho(ADC,ADC_{-1})",2);
    p_coeffs[k]->setBinLabel(7,"k(ADC,ADC_{-1})",2);
    p_coeffs[k]->setBinLabel(8,"o(ADC,ADC_{-1})",2);
    p_coeffs[k]->setBinLabel(9,"#rho(ADC,TDC)",2);
    p_coeffs[k]->setBinLabel(10,"k(ADC,TDC)",2);
    p_coeffs[k]->setBinLabel(11,"o(ADC,TDC)",2);   
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

    for(auto kit : hex_pedestal) { 
      MonitorKey_t k(kit.first);
      //hex_channelId[k]->addBin(gr);
      //hex_channelId[k]->setBinContent(i+1, i==0 ? 1e-6 : i);
      hex_pedestal[k]->addBin(gr);
      hex_noise[k]->addBin(gr);
      hex_cmrho[k]->addBin(gr);
      hex_bxm1rho[k]->addBin(gr);
    }

    i++;    
  }
  fgeo->Close();

  edm::LogInfo("HGCalDigisClientHarvester") << "Retrieved : " << module_keys_.size() << " module tags to harvest and defined hexmaps";


  //loop over modules to harvest
  std::map<HGCalElectronicsId,hgcal::CellStatistics> summary_stats;
  for(auto it : module_keys_) {
    
    MonitorKey_t k(it.second);
    TString tag=Form("%d_%d_%d_%d",std::get<0>(k),std::get<1>(k),std::get<2>(k),std::get<3>(k));

    std::string meName("HGCAL/Digis/sums_"+tag);
    const MonitorElement *me = igetter.get(meName);
    if (me == nullptr) continue;
    std::cout << meName << " " << me->getEntries() << std::endl;
    //convert the sums to coefficients
    for(int ibin=1; ibin< me->getNbinsX(); ibin++) {
      hgcal::CellStatistics stats;
      stats.n      = me->getBinContent(ibin,1);
      stats.sum_x  = me->getBinContent(ibin,2);
      stats.sum_xx  = me->getBinContent(ibin,3);
      stats.sum_s  = {me->getBinContent(ibin,4),me->getBinContent(ibin,7),me->getBinContent(ibin,10)};
      stats.sum_ss = {me->getBinContent(ibin,5),me->getBinContent(ibin,8),me->getBinContent(ibin,11)};
      stats.sum_xs = {me->getBinContent(ibin,6),me->getBinContent(ibin,9),me->getBinContent(ibin,12)};
      //std::cout << stats.n << " " << stats.sum_x << " " << stats.sum_xx << std::endl;
      
      std::pair<double,double> obs = stats.getObservableStats();
      std::vector<double> Rs = stats.getPearsonCorrelation();
      std::vector<double> slopes = stats.getSlopes();
      std::vector<double> intercepts = stats.getIntercepts();
      p_coeffs[k]->setBinContent(ibin,1,obs.first);
      p_coeffs[k]->setBinContent(ibin,2,obs.second);
      for(size_t i=0; i<3; i++) {
        p_coeffs[k]->setBinContent(ibin,3+3*i,Rs[i]);
        p_coeffs[k]->setBinContent(ibin,4+3*i,slopes[i]);
        p_coeffs[k]->setBinContent(ibin,5+3*i,intercepts[i]);
      }

      //fill hexplots
      hex_pedestal[k]->setBinContent(ibin,obs.first);
      hex_noise[k]->setBinContent(ibin,obs.second);
      hex_cmrho[k]->setBinContent(ibin,Rs[0]);
      hex_bxm1rho[k]->setBinContent(ibin,Rs[1]);
      
      //add to summary stats
      bool zside = std::get<0>(k);
      uint16_t slink = std::get<1>(k);
      uint16_t captureblock = std::get<2>(k);
      uint16_t econd = std::get<3>(k);
      uint16_t erx = (ibin-1)/39;
      uint16_t seq = (ibin-1)%39;
      HGCalElectronicsId eleid(zside,slink,captureblock,econd,erx,seq);      
      summary_stats[eleid]=stats;     
    }
    
  }

  //save pedestals to file
  export_calibration_parameters(summary_stats);
}

//
void HGCalDigisClientHarvester::export_calibration_parameters(std::map<HGCalElectronicsId,hgcal::CellStatistics> &stats) {

  //open txt file
  std::ofstream myfile(level0CalibOut_);

  myfile << "Channel Pedestal Noise CM_slope CM_offset BXm1_slope BXm1_offset\n";
  for(std::map<HGCalElectronicsId,hgcal::CellStatistics>::iterator it=stats.begin();
      it!=stats.end(); it++) {

    HGCalElectronicsId id=it->first;
    hgcal::CellStatistics s=it->second;
    std::pair<double,double> obs = s.getObservableStats();
    std::vector<double> slopes = s.getSlopes();
    std::vector<double> intercepts = s.getIntercepts();
    
    myfile << "0x" << std::hex << id.raw() << " " << std::dec
           << std::setprecision(3)
           << obs.first << " " << obs.second << " "
           << slopes[0] << " " << intercepts[0] << " "
           << slopes[1] << " " << intercepts[1] << " "
           << std::endl;
  }

  myfile.close();

  edm::LogInfo("HGCalDigisClient") << "Export CM parameters @ " << level0CalibOut_ << std::endl;
}

//
void HGCalDigisClientHarvester::dqmEndJob(DQMStore::IBooker& ibooker, DQMStore::IGetter& igetter)
{

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HGCalDigisClientHarvester);
