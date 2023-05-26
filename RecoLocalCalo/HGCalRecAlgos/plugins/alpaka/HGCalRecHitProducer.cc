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

  class HGCalRecHitProducer : public edm::stream::EDProducer<> {
  public:
    explicit HGCalRecHitProducer(const edm::ParameterSet&);
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    std::unique_ptr<HGCalDeviceDigiCollection> hostToDevice(const std::unique_ptr<HGCalHostDigiCollection> &hostDigis);
    std::unique_ptr<HGCalHostRecHitCollection> deviceToHost(const std::unique_ptr<HGCalDeviceRecHitCollection> &deviceRecHits);

    const edm::EDGetTokenT<HGCalElecDigiCollection> elecDigisToken_;
    const edm::EDPutTokenT<std::unique_ptr<HGCalHostRecHitCollection>> recHitsToken_;
    const std::unique_ptr<HGCalRecHitCalibrationAlgorithms> calibrator;
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig)
      : elecDigisToken_(consumes<HGCalElecDigiCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
        recHitsToken_(produces<std::unique_ptr<HGCalHostRecHitCollection>>()),
        calibrator(std::make_unique<HGCalRecHitCalibrationAlgorithms>())
  {}

  void HGCalRecHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
  {
    // Read digis
    auto elecDigis = iEvent.get(elecDigisToken_); // this ultimately should return HGCalHostRecHitCollection
    auto hostDigis = std::make_unique<HGCalHostDigiCollection>(); // = iEvent.get(elecDigisToken_);

    std::unique_ptr<HGCalDeviceDigiCollection> deviceDigis = hostToDevice(hostDigis);
    std::unique_ptr<HGCalDeviceRecHitCollection> deviceRecHits = calibrator->calibrate(deviceDigis);
    std::unique_ptr<HGCalHostRecHitCollection> hostRecHits = deviceToHost(deviceRecHits);

    iEvent.emplace(recHitsToken_, std::move(hostRecHits));
  }

  std::unique_ptr<HGCalDeviceDigiCollection> HGCalRecHitProducer::hostToDevice(const std::unique_ptr<HGCalHostDigiCollection> &hostDigis)
  {
    auto deviceDigis = std::make_unique<HGCalDeviceDigiCollection>();
    // populate device digis from host digis
    return deviceDigis;
  }

  std::unique_ptr<HGCalHostRecHitCollection> HGCalRecHitProducer::deviceToHost(const std::unique_ptr<HGCalDeviceRecHitCollection> &deviceRecHits)
  {
    auto hostRecHits = std::make_unique<HGCalHostRecHitCollection>();
    // populate host rec hits from device rec hits
    return hostRecHits;
  }


  void HGCalRecHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("Digis", edm::InputTag("hgcalDigis", "DIGI", "TEST"));
    descriptions.addWithDefaultLabel(desc);
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in

DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
