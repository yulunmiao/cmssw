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

// includes for loading pedestal txt file
#include <iomanip> // for std::setw
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "CondFormats/DataRecord/interface/HGCalCondSerializablePedestalsRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializablePedestals.h"
#include "DataFormats/Math/interface/libminifloat.h"

#include "CondFormats/DataRecord/interface/HGCalCondSerializableGenericConfigRcd.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableGenericConfig.h"

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

    edm::ESWatcher<HGCalCondSerializablePedestalsRcd> cfgWatcher_;
    edm::ESGetToken<HGCalCondSerializablePedestals, HGCalCondSerializablePedestalsRcd> tokenConds_;
    const edm::EDGetTokenT<HGCalDigiHostCollection> digisToken_;
    const device::EDPutToken<HGCalRecHitDeviceCollection> recHitsToken_;
    HGCalRecHitCalibrationAlgorithms calibrator_;  // cannot be "const" because the calibrate() method is not const
    int n_hits_scale;
  };

  HGCalRecHitProducer::HGCalRecHitProducer(const edm::ParameterSet& iConfig)
      : tokenConds_(esConsumes<HGCalCondSerializablePedestals, HGCalCondSerializablePedestalsRcd>(
          edm::ESInputTag(iConfig.getParameter<std::string>("pedestal_label")))),
        digisToken_{consumes<HGCalDigiHostCollection>(iConfig.getParameter<edm::InputTag>("digis"))},
        recHitsToken_{produces()},
        calibrator_{HGCalRecHitCalibrationAlgorithms(
          iConfig.getParameter<int>("n_blocks"),
          iConfig.getParameter<int>("n_threads"))},
        n_hits_scale{iConfig.getParameter<int>("n_hits_scale")}
    {}

  void HGCalRecHitProducer::produce(device::Event& iEvent, device::EventSetup const& iSetup) {
    auto queue = iEvent.queue();

    std::cout << "\n\nINFO -- Start of produce\n\n" << std::endl;
    
    // Read digis
    auto const& hostDigisIn = iEvent.get(digisToken_);
    int oldSize = hostDigisIn.view().metadata().size();
    int newSize = oldSize * n_hits_scale;
    auto hostDigis = HGCalDigiHostCollection(newSize, queue);

    // Check if there are new conditions and read them
    if (cfgWatcher_.check(iSetup)){

      auto conds = iSetup.getData(tokenConds_);
      size_t nconds = conds.params_.size();
      edm::LogInfo("HGCalPedestalsESSourceAnalyzer") << "Conditions retrieved:\n" << nconds;

      // Print out all conditions readout
      HGCalRecHitCalibrationAlgorithms::CalibParams calibParams;
      std::cout << "   ID  eRx  ROC  Channel  isCM?  Pedestal  CM slope  CM offset  kappa(BX-1)" << std::endl;
      for(auto it : conds.params_) {
        HGCalElectronicsId id(it.first);
        bool cmflag = id.isCM();
        uint32_t eRx = (uint32_t) id.econdeRx();
        uint32_t roc = (uint32_t) eRx/2;
        uint32_t ch = id.halfrocChannel();
        HGCalPedestals table(it.second);
        float pedestal = MiniFloatConverter::float16to32(table.pedestal);
        float cm_slope = MiniFloatConverter::float16to32(table.cm_slope);
        float cm_offset = MiniFloatConverter::float16to32(table.cm_offset);
        float kappa_bxm1 = MiniFloatConverter::float16to32(table.kappa_bxm1);
        calibParams[id] = pedestal;
        std::cout << std::setw(5) << std::hex << id.raw() << " " << std::setw(4) << std::dec << eRx << " "
                  << std::setw(4) << roc << " " << std::setw(8) << ch << " " << std::setw(6) << cmflag << " "
                  << std::setw(9) << std::setprecision(3) << pedestal << " " << std::setw(9) << cm_slope << " "
                  << std::setw(10) << cm_offset << " " << std::setw(12) << kappa_bxm1 << std::endl;
      }

      calibrator_.loadCalibParams(calibParams); // TODO: load map: electronicsID -> vector { pedestal }

    }

    for(int i=0; i<newSize;i++){
      hostDigis.view()[i].electronicsId() = hostDigisIn.view()[i%oldSize].electronicsId();
      hostDigis.view()[i].tctp() = hostDigisIn.view()[i%oldSize].tctp();
      hostDigis.view()[i].adcm1() = hostDigisIn.view()[i%oldSize].adcm1();
      hostDigis.view()[i].adc() = hostDigisIn.view()[i%oldSize].adc();
      hostDigis.view()[i].tot() = hostDigisIn.view()[i%oldSize].tot();
      hostDigis.view()[i].toa() = hostDigisIn.view()[i%oldSize].toa();
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
    desc.add<std::string>("pedestal_label", "");
    descriptions.addWithDefaultLabel(desc);
  }

}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// define this as a plug-in
#include "HeterogeneousCore/AlpakaCore/interface/alpaka/MakerMacros.h"
DEFINE_FWK_ALPAKA_MODULE(HGCalRecHitProducer);
