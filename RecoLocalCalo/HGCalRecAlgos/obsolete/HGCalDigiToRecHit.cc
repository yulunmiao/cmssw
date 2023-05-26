#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/HGCalDigi/interface/HGCROCChannelDataFrame.h"
#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"

#include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/HGCalCalibration.h"
class HGCalDigiToRecHit : public edm::stream::EDProducer<> {
public:
  explicit HGCalDigiToRecHit(const edm::ParameterSet&);
  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;

  const edm::EDGetTokenT<HGCalElecDigiCollection> elecDigisToken_;
  const edm::EDPutTokenT<HGCRecHitCollection> recHitsToken_;
  const std::unique_ptr<HGCalCalibration> calibration_;

};

HGCalDigiToRecHit::HGCalDigiToRecHit(const edm::ParameterSet& iConfig)
    : elecDigisToken_(consumes<HGCalElecDigiCollection>(iConfig.getParameter<edm::InputTag>("Digis"))),
      recHitsToken_(produces<HGCRecHitCollection>()),
      calibration_() {

}
void HGCalDigiToRecHit::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
    HGCRecHitCollection recHits;
    const auto& elecDigis = iEvent.get(elecDigisToken_);
    for (auto& elecDigi : elecDigis) {
        std::cout<<"Electronics Id="<<elecDigi.id().raw()<<std::endl;
        elecDigi.print();
        HGCRecHit recHit = calibration_->calibrate(elecDigi,0);
        recHits.push_back(recHit);
        std::cout<<"DetId="<<recHit.id().rawId()<<",energy="<<recHit.energy()<<",time="<<recHit.time()<<std::endl;
    }
  iEvent.emplace(recHitsToken_, std::move(recHits));
}

void HGCalDigiToRecHit::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("Digis",edm::InputTag("hgcalDigis","DIGI","TEST"));
  descriptions.add("hgcalRecHits", desc);

}

// define this as a plug-in
DEFINE_FWK_MODULE(HGCalDigiToRecHit);