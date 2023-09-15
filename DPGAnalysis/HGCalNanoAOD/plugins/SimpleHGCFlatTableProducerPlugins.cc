#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
typedef SimpleFlatTableProducer<CaloRecHit> SimpleCaloRecHitFlatTableProducer;

#include "DataFormats/HGCalDigi/interface/HGCalDigiCollections.h"
typedef SimpleFlatTableProducer<HGCROCChannelDataFrameElecSpec> SimpleHGCalDigiFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimpleCaloRecHitFlatTableProducer);
DEFINE_FWK_MODULE(SimpleHGCalDigiFlatTableProducer);
