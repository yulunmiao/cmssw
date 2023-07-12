import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("TESTDQM")

options = VarParsing.VarParsing()
options.parseArguments()


process.load("FWCore.MessageService.MessageLogger_cfi")

process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(options.inputFiles))

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

process.hgCalDigisClient = cms.EDProducer(
    'HGCalDigisClient',
    Digis=cms.InputTag('hgcalDigis', ''),
    MetaData=cms.InputTag('hgcalEmulatedSlinkRawData', 'hgcalMetaData'),
    ModuleMapping=cms.ESInputTag(''),
)

process.hgCalDigisClientHarvester = cms.EDProducer(
    'HGCalDigisClientHarvester',
    ModuleMapping=process.hgCalDigisClient.ModuleMapping,
    HexTemplateFile=cms.string('/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/hexagons_20230626.root'),
    Level0CalibOut=cms.string('level0_calib_params.txt'),
)

process.DQMStore = cms.Service("DQMStore")

process.load("DQMServices.FileIO.DQMFileSaverOnline_cfi")
process.dqmSaver.tag = 'HGCAL'

# path
process.p = cms.Path(process.hgCalDigisClient * process.hgCalDigisClientHarvester * process.dqmSaver)
