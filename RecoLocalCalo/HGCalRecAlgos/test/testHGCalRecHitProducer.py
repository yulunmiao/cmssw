import FWCore.ParameterSet.Config as cms

process = cms.Process('HGCalRecHitProducerTest')

process.source = cms.Source('EmptySource')

process.load('Configuration.StandardSequences.Accelerators_cff')
process.load('HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi')

#--------------------------------------------------
# Message logger
#--------------------------------------------------
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.MessageLogger.cerr.INFO.limit = cms.untracked.int32(10000000)

# process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/y/yumiao/public/HGCAL_Raw_Data_Handling/Data/Digis/testFakeDigisSoA.root')
# )






#-------------------------------------------------------------------------------------
# running on the best backend, i.e.
# run the producer on a gpu and copy the product to the cpu, or run directly on the cpu
#-------------------------------------------------------------------------------------
# process.testProducer = cms.EDProducer('HGCalRecHitProducer@alpaka')
# process.testProducer = cms.EDProducer('HGCalRecHitProducer')

# process.testProducer = cms.EDProducer('HGCalRecHitProducer'
#                                     #   ,size = cms.int32(42)
# )

# process.testAnalyzer = cms.EDAnalyzer('TestAlpakaRecHitAnalyzer',
#     source = cms.InputTag('testProducer')
# )

#--------------------------------------------------
# run the producer explicitly on the cpu
#--------------------------------------------------
process.testProducerSerial = cms.EDProducer('alpaka_serial_sync::HGCalRecHitProducer')

# process.testAnalyzerSerial = cms.EDAnalyzer('TestAlpakaRecHitAnalyzer',
#     source = cms.InputTag('testProducerSerial')
# )

#--------------------------------------------------
# write the product to a 'output_test.root' file
#--------------------------------------------------
# process.output = cms.OutputModule('PoolOutputModule',
#     fileName = cms.untracked.string('output_test.root'),
#     outputCommands = cms.untracked.vstring(
#         'drop *',
#         #'keep *_testProducer_*_*',
#         'keep *_testProducerSerial_*_*',
#     )
# )


process.output = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string("./recHits.root"),
)

#process.gpu_path = cms.Path(process.testProducer + process.testAnalyzer)
# process.serial_path = cms.Path(process.testProducerSerial + process.testAnalyzerSerial)

# process.gpu_path = cms.Path(process.testProducer)
process.serial_path = cms.Path(process.testProducerSerial)
process.output_path = cms.EndPath(process.output)

process.maxEvents.input = 1
