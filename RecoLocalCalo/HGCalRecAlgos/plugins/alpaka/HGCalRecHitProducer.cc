// CMSSW includes
#include "DataFormats/HGCalDigi/interface/HGCalDigiHostCollection.h"
#include "DataFormats/HGCalDigi/interface/alpaka/HGCalDigiDeviceCollection.h"
#include "DataFormats/HGCalRecHit/interface/HGCalRecHitHostCollection.h"
#include "DataFormats/HGCalRecHit/interface/alpaka/HGCalRecHitDeviceCollection.h"
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

#include <future>

template<class T> double duration(T t0,T t1)
{
  auto elapsed_secs = t1-t0;
  typedef std::chrono::duration<float> float_seconds;
  auto secs = std::chrono::duration_cast<float_seconds>(elapsed_secs);
  return secs.count();
}

inline std::chrono::time_point<std::chrono::steady_clock> now()
{
  return std::chrono::steady_clock::now();
}

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using namespace cms::alpakatools;

  class HGCalRecHitProducer : public stream::EDProducer<> {
  public:
    explicit HGCalRecHitProducer(const edm::ParameterSet&);
    static void fillDescriptions(edm::ConfigurationDescriptions&);

  private:
    void produce(device::Event&, device::EventSetup const&) override;

    const edm::EDGetTokenT<HGCalDigiHostCollection> digisToken_;
    const device::EDPutToken<HGCalRecHitDeviceCollection> recHitsToken_;
    HGCalRecHitCalibrationAlgorithms calibrator_;  // cannot be "const" because the calibrate() method is not const
    int n_hits_scale;
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig)
      : digisToken_{consumes<HGCalDigiHostCollection>(iConfig.getParameter<edm::InputTag>("digis"))},
        recHitsToken_{produces()},
        calibrator_{HGCalRecHitCalibrationAlgorithms(
          iConfig.getParameter<int>("n_blocks"),
          iConfig.getParameter<int>("n_threads"))},
          n_hits_scale{iConfig.getParameter<int>("n_hits_scale")} {}

  void HGCalRecHitProducer::produce(device::Event& iEvent, device::EventSetup const& iSetup) {
    auto queue = iEvent.queue();

    std::cout << "\n\nINFO -- Start of produce\n\n" << std::endl;
    // Read digis
    auto const& hostDigisIn = iEvent.get(digisToken_);
    int oldSize = hostDigisIn.view().metadata().size();
    int newSize = oldSize * n_hits_scale;
    auto hostDigis = HGCalDigiHostCollection(newSize, queue);

    for(int i=0; i<newSize;i++){
      hostDigis.view()[i].electronicsId() = hostDigisIn.view()[i%oldSize].electronicsId();
      // hostDigis.view()[i].raw() = hostDigisIn.view()[i%oldSize].raw();
      hostDigis.view()[i].raw() = i;
      hostDigis.view()[i].cm() = hostDigisIn.view()[i%oldSize].cm();
      hostDigis.view()[i].flags() = hostDigisIn.view()[i%oldSize].flags();
    }

    std::cout << "Loaded host digis: " << hostDigis.view().metadata().size() << std::endl;

    std::cout << "\n\nINFO -- calling calibrate method" << std::endl;

    auto start = now();
    auto recHits = calibrator_.calibrate(queue, hostDigis);
    auto stop = now();

    std::cout<<"Time: "<< duration(start, stop) <<std::endl;

    std::cout << "\n\nINFO -- storing rec hits in the event" << std::endl;
    // iEvent.put(recHitsToken_, std::move(recHits));
  }

  void HGCalRecHitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("digis", edm::InputTag("hgcalDigis", "DIGI", "TEST"));
    desc.add<int>("n_blocks", -1);
    desc.add<int>("n_threads", -1);
    desc.add<int>("n_hits_scale", -1);
    descriptions.addWithDefaultLabel(desc);
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
