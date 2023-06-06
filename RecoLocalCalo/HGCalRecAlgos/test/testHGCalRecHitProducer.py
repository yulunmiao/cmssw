import FWCore.ParameterSet.Config as cms

run_on_gpu = False

process = cms.Process('HGCalRecHitProducerTest')

process.source = cms.Source("PoolSource",
   fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/y/yumiao/public/HGCAL_Raw_Data_Handling/Data/Digis/testFakeDigisSoA.root')
)

process.load('Configuration.StandardSequences.Accelerators_cff')
process.load('HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi')
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.MessageLogger.cerr.INFO.limit = cms.untracked.int32(10000000)

# This doesn't work for some reason, so using a switch "run_on_gpu" for now
# process.testProducer = cms.EDProducer(
#     'HGCalRecHitProducer@alpaka',
#     alpaka = cms.untracked.Pset( backend = cms.untracked.string("serial_sync"))
# )

process.producerGpu = cms.EDProducer('alpaka_cuda_async::HGCalRecHitProducer')
process.producerCpu = cms.EDProducer('alpaka_serial_sync::HGCalRecHitProducer')

process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("./recHits.root"),
#     outputCommands = cms.untracked.vstring(
#         'drop *',
#         'keep *_producerGpu_*_*',
#         'keep *_producerCpu_*_*',
#
)

process.path = cms.Path(process.producerGpu if run_on_gpu else process.producerCpu)
process.output_path = cms.EndPath(process.output)

process.maxEvents.input = 1
