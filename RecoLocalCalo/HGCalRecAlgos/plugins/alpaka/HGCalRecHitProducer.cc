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

// HGCal digis and RecHits
#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"
#include "DataFormats/HGCalDigi/interface/HGCalHostDigiCollection.h"
#include "DataFormats/HGCalRecHit/interface/HGCalHostRecHitCollection.h"

#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDeviceDigiCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalDeviceRecHitCollection.h"

// Alpaka-based calibration class
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"


namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace std;

  class HGCalRecHitProducer : public edm::stream::EDProducer<> {
  public:
    explicit HGCalRecHitProducer(const edm::ParameterSet&);
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    // const edm::EDGetTokenT<HGCalElecDigiCollection> elecDigisToken_;
    const edm::EDPutTokenT<HGCalDeviceRecHitCollection> recHitsToken_;
    const std::unique_ptr<HGCalRecHitCalibrationAlgorithms> calibrator;
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig): 
        // elecDigisToken_(consumes<HGCalElecDigiCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
        recHitsToken_(produces<HGCalDeviceRecHitCollection>()),
        calibrator(std::make_unique<HGCalRecHitCalibrationAlgorithms>())
  {}

  void HGCalRecHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {
    cout<<"\n\nINFO -- Start of produce\n\n"<<endl;
    // Read digis
    // auto elecDigis = iEvent.get(elecDigisToken_); // this ultimately should return HGCalHostRecHitCollection
    
    // Filling host digis with some dummy values
    cout<<"INFO -- Creating digis collection"<<endl;
    auto digis = std::make_unique<HGCalDeviceDigiCollection>(); // = iEvent.get(elecDigisToken_);

    // //          electronicsID raw cm flags
    // digis->view()[0] = {0, 10 , 1 , 0};
    // digis->view()[1] = {1, 9  , 2 , 0};
    // digis->view()[2] = {2, 11 , 1 , 0};
    // digis->view()[3] = {3, 10 , 1 , 0};

    cout<<"INFO -- calling calibrate method"<<endl;
    std::unique_ptr<HGCalDeviceRecHitCollection> recHits = calibrator->calibrate(digis);
    
    cout<<"INFO -- storing rec hits in the event"<<endl;
    iEvent.emplace(recHitsToken_, std::move(*recHits));
  }

  void HGCalRecHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    // desc.add<edm::InputTag>("Digis", edm::InputTag("hgcalDigis", "DIGI", "TEST"));
    descriptions.addWithDefaultLabel(desc);
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in

DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
