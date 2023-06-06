// Typical CMSSW imports
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"

// Alpaka-related imports
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"

// HGCal digis and RecHits
#include "DataFormats/HGCalDigi/interface/HGCalHostDigiCollection.h"
#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDeviceDigiCollection.h"

#include "DataFormats/HGCalRecHit/interface/HGCalHostRecHitCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalDeviceRecHitCollection.h"

// Alpaka-based calibration class
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"


namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;
  using namespace std;

  class HGCalRecHitProducer : public edm::stream::EDProducer<> {
  public:
    explicit HGCalRecHitProducer(const edm::ParameterSet&);
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    const edm::EDGetTokenT<HGCalHostDigiCollection> digisToken_;
    const edm::EDPutTokenT<HGCalDeviceRecHitCollection> recHitsToken_;
    const std::unique_ptr<HGCalRecHitCalibrationAlgorithms> calibrator;
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig) :
    digisToken_(consumes<HGCalHostDigiCollection>(iConfig.getParameter<edm::InputTag>("digis"))),
    recHitsToken_{produces()},
    calibrator(std::make_unique<HGCalRecHitCalibrationAlgorithms>())
  {}

  void HGCalRecHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {
    cout<<"\n\nINFO -- Start of produce\n\n"<<endl;
    // Read digis
    auto const& hostDigis = iEvent.get(digisToken_);
    cout<<"Loaded host digis: "<<hostDigis.view().metadata().size()<<endl;

    cout<<"\n\nINFO -- calling calibrate method"<<endl;
    std::unique_ptr<HGCalDeviceRecHitCollection> recHits = calibrator->calibrate(hostDigis);
    
    cout<<"\n\nINFO -- storing rec hits in the event"<<endl;
    iEvent.emplace(recHitsToken_, std::move(*recHits));
  }

  void HGCalRecHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("digis", edm::InputTag("hgcalDigis", "DIGI", "TEST"));
    descriptions.addWithDefaultLabel(desc);
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in
DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
