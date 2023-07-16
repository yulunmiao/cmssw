#include "DPGAnalysis/HGCalNanoAOD/interface/DigiInfoTableProducer.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"

#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"

#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"

template <typename T1, typename T2>
class HGCalDigiFromDetIDTableProducer : public DigiInfoTableProducer< edm::View<CaloRecHit>,edm::View<HGCROCChannelDataFrameElecSpec> > {
 public:
  HGCalDigiFromDetIDTableProducer(edm::ParameterSet const& params)
    : DigiInfoTableProducer(params) {
  }
  ~HGCalDigiFromDetIDTableProducer() override {}

  bool findMatch(const CaloRecHit& hit, const HGCROCChannelDataFrameElecSpec& digi, const std::map<uint32_t,uint32_t> & ele2geo_) {

    bool foundMatch = false;
    const HGCalElectronicsId  eleId = digi.id();
    for(auto it : ele2geo_) {
      if (it.first==eleId.raw() and it.second==hit.detid().rawId()) foundMatch = true;
    }
    return foundMatch;
  }


  float getTOA(const HGCROCChannelDataFrameElecSpec& digi) {
    return digi.toa();
  }

  float getTOT(const HGCROCChannelDataFrameElecSpec& digi) {
    return digi.tot();
  }

  float getADC(const HGCROCChannelDataFrameElecSpec& digi) {
    return digi.adc();
  }

  float getADCm1(const HGCROCChannelDataFrameElecSpec& digi) {
    return digi.adcm1();
  }

};

#include "FWCore/Framework/interface/MakerMacros.h"
typedef HGCalDigiFromDetIDTableProducer<HGCRecHitCollection,HGCalElecDigiCollection> HGCRecHitDigiTableProducer;
DEFINE_FWK_MODULE(HGCRecHitDigiTableProducer);
