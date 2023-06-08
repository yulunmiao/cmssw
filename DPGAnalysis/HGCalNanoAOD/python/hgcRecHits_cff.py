import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import Var,P3Vars

hgcEERecHitsTable = cms.EDProducer("SimpleCaloRecHitFlatTableProducer",
    src = cms.InputTag("HGCalRecHit:HGCEERecHits"),
    cut = cms.string(""), 
    name = cms.string("RecHitHGCEE"),
    doc  = cms.string("RecHits in HGCAL Electromagnetic endcap"),
    singleton = cms.bool(False), # the number of entries is variable
    extension = cms.bool(False), # this is the main table for the muons
    variables = cms.PSet(
        detId = Var('detid().rawId()', 'int', precision=-1, doc='detId'),
        energy = Var('energy', 'float', precision=14, doc='energy'),
        time = Var('time', 'float', precision=14, doc='hit time'),
    )
)

hgcEERecHitsPositionTable = cms.EDProducer("HGCRecHitPositionTableProducer",
     src = hgcEERecHitsTable.src,
     cut = hgcEERecHitsTable.cut,
     name = hgcEERecHitsTable.name,
     doc  = hgcEERecHitsTable.doc,
 )

hgcHEfrontRecHitsTable = hgcEERecHitsTable.clone()
hgcHEfrontRecHitsTable.src = "HGCalRecHit:HGCHEFRecHits"
hgcHEfrontRecHitsTable.name = "RecHitHGCHEF"

hgcHEfrontRecHitsPositionTable = cms.EDProducer("HGCRecHitPositionTableProducer",
     src = hgcHEfrontRecHitsTable.src,
     cut = hgcHEfrontRecHitsTable.cut,
     name = hgcHEfrontRecHitsTable.name,
     doc  = hgcHEfrontRecHitsTable.doc,
 )

hgcHEbackRecHitsTable = hgcEERecHitsTable.clone()
hgcHEbackRecHitsTable.src = "HGCalRecHit:HGCHEBRecHits"
hgcHEbackRecHitsTable.name = "RecHitHGCHEB"

hgcRecHitsTask = cms.Task(hgcEERecHitsTable,hgcHEfrontRecHitsTable,hgcHEbackRecHitsTable,hgcEERecHitsPositionTable,hgcHEfrontRecHitsPositionTable)
