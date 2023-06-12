// CMSSW includes
#include "DataFormats/HGCalDigi/interface/HGCalHostDigiCollection.h"
#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDeviceDigiCollection.h"
#include "DataFormats/HGCalRecHit/interface/HGCalHostRecHitCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalDeviceRecHitCollection.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/EDPutToken.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/Event.h"
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/stream/EDProducer.h"
#include "HeterogeneousCore/AlpakaInterface/interface/CopyToDevice.h"
#include "RecoLocalCalo/HGCalRecAlgos/plugins/alpaka/HGCalRecHitCalibrationAlgorithms.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;

  class HGCalRecHitProducer : public stream::EDProducer<> {
  public:
    explicit HGCalRecHitProducer(const edm::ParameterSet&);
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void produce(device::Event&, device::EventSetup const&) override;

    const edm::EDGetTokenT<HGCalHostDigiCollection> digisToken_;
    const device::EDPutToken<HGCalDeviceRecHitCollection> recHitsToken_;
    HGCalRecHitCalibrationAlgorithms calibrator_;  // cannot be "const" because the calibrate() method is not const
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig)
      : digisToken_{consumes<HGCalHostDigiCollection>(iConfig.getParameter<edm::InputTag>("digis"))},
        recHitsToken_{produces()},
        calibrator_{} {}

  void HGCalRecHitProducer::produce(device::Event& iEvent, device::EventSetup const& iSetup) {
    auto queue = iEvent.queue();

    std::cout << "\n\nINFO -- Start of produce\n\n" << std::endl;
    // Read digis
    auto const& hostDigis = iEvent.get(digisToken_);
    std::cout << "Loaded host digis: " << hostDigis.view().metadata().size() << std::endl;

    std::cout << "\n\nINFO -- calling calibrate method" << std::endl;
    auto recHits = calibrator_.calibrate(queue, hostDigis);

    std::cout << "\n\nINFO -- storing rec hits in the event" << std::endl;
    iEvent.put(recHitsToken_, std::move(recHits));
  }

  void HGCalRecHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("digis", edm::InputTag("hgcalDigis", "DIGI", "TEST"));
    descriptions.addWithDefaultLabel(desc);
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
