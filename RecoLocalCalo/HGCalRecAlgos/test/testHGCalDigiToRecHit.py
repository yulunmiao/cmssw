import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("testRecHit")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:test.root')
)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

process.load("RecoLocalCalo.HGCalRecAlgos.hgcalRecHits_cfi")

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("./testRecHit.root"),
)
process.outpath = cms.EndPath(process.output)
process.p = cms.Path(process.hgcalRecHits)

