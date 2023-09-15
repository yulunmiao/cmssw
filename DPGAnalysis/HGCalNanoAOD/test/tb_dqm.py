import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

process = cms.Process("TESTDQM")

options = VarParsing('analysis')
options.register('minEvents', 10000, VarParsing.multiplicity.singleton, VarParsing.varType.int, "min. events to process sequentially")
options.register('prescale',  1000,  VarParsing.multiplicity.singleton, VarParsing.varType.int, "prescale every N events")
options.parseArguments()


process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 50000
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(options.inputFiles))

process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(options.maxEvents))

# Logical mapping
process.load('Geometry.HGCalMapping.hgCalModuleInfoESSource_cfi')
process.load('Geometry.HGCalMapping.hgCalSiModuleInfoESSource_cfi')
from DPGAnalysis.HGCalTools.tb2023_cfi import configTBConditions,addPerformanceReports
configTBConditions(process)
addPerformanceReports(process)

process.hgCalDigisClient = cms.EDProducer(
    'HGCalDigisClient',
    Digis=cms.InputTag('hgcalDigis', ''),
    FlaggedECONDInfo=cms.InputTag("hgcalDigis","UnpackerFlags"),
    MetaData=cms.InputTag('hgcalEmulatedSlinkRawData', 'hgcalMetaData'),
    ModuleMapping=cms.ESInputTag(''),
    Prescale=cms.uint32(options.prescale),
    MinimumNumEvents=cms.uint32(options.minEvents),
)
process.hgCalDigisClientHarvester = cms.EDProducer(
    'HGCalDigisClientHarvester',
    ModuleMapping=process.hgCalDigisClient.ModuleMapping,
    HexTemplateFile=cms.string('/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/hexagons_20230801.root'),
    Level0CalibOut=cms.string('level0_calib_params.txt'),
)

process.DQMStore = cms.Service("DQMStore")

process.load("DQMServices.FileIO.DQMFileSaverOnline_cfi")
process.dqmSaver.tag = 'HGCAL'

# path
process.p = cms.Path(process.hgCalDigisClient * process.hgCalDigisClientHarvester * process.dqmSaver)
