import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("TEST")

options = VarParsing.VarParsing('standard')
options.register('mode', 'hgcmodule', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,
                 'type of emulation')
options.register('fedId', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'emulated FED id')
options.register('debug', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'debugging mode')
options.register('dumpFRD', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'also dump the FEDRawData content')
options.register('numCaptureBlocks', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'number of capture blocks to emulate')
options.register('numECONDs', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'number of ECON-Ds to emulate')
options.register('numChannelsPerERx', 37, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'number of channels enabled per ERx')
options.register('numERxsPerECOND', 6, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'number of ERxs enabled per ECON-D')
options.register('activeECONDs', [0], VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.int,
                 'list of ECON-Ds enabled')
options.register('ECONDsInPassthrough', [0], VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.int,
                 'list of ECON-Ds in passthrough mode')
options.register('ECONDsInCharacterisation', [], VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.int,
                 'list of ECON-Ds in characterisation mode')
options.register('ECONDToTStatus', 3, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'default ToT status bits (aka TcTp bits) value to be emulated')
options.register('randomActiveCaptureBlocks', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'randomly activate capture blocks on emulation')
options.register('randomActiveECOND', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'randomly activate ECOn-Ds on emulation')
options.register('storeOutput', True, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'also store the output into an EDM file')
options.register('storeRAWOutput', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'also store the RAW output into a streamer file')
options.register('storeEmulatorInfo', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'also store the emulator metadata')
options.register('configFile',
                 '/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/calibration_module815/calib_withOct2022/80fC/80fC_inj_lowgain_loop_module815_beamtest/pedestal_run/run_20230412_160049/pedestal_run0.yaml',
                 VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,
                 'config yaml file')
options.register('pedestalFile',
                 '/afs/cern.ch/work/y/ykao/public/raw_data_handling/calibration_parameters.txt',
                 VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,
                 'pedestal txt file')
options.register('inputFiles',
                 #'file:/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/calibration_module815/calib_withOct2022/160fC/160fC_inj_lowgain_loop_module815_beamtest/pedestal_run/run_20230412_141543/pedestal_run0.root',
                 'file:/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/labtest/module822/pedestal_run0.root',
                 VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string,
                 'input TB file')
options.register('GPU', False, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,
                 'run on GPU')
options.maxEvents = 100  # number of events to emulate
options.output = 'output.root'  # output EDM file
options.secondaryOutput = 'output.raw'  # output streamer file
options.parseArguments()

process.load('EventFilter.HGCalRawToDigi.hgcalEmulatedSlinkRawData_cfi')
process.load('EventFilter.HGCalRawToDigi.hgcalDigis_cfi')
process.load('CalibCalorimetry.HGCalPlugins.hgCalConfigESSourceFromYAML_cfi') # read yaml config file(s)
process.load('CalibCalorimetry.HGCalPlugins.hgCalPedestalsESSource_cfi') # read txt pedestal file for calibration
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.Accelerators_cff')
process.load('HeterogeneousCore.AlpakaCore.ProcessAcceleratorAlpaka_cfi')
if options.debug:
    process.MessageLogger.cerr.threshold = "DEBUG"
    process.MessageLogger.debugModules = ["*"]
    process.MessageLogger.cerr.DEBUG = cms.untracked.PSet(
        limit = cms.untracked.int32(-1)
    )


process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(options.maxEvents))
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    hgcalEmulatedSlinkRawData = cms.PSet(initialSeed = cms.untracked.uint32(42))
)

#process.source = cms.Source("EmptySource")
process.source = cms.Source('EmptyIOVSource',
    timetype = cms.string('runnumber'),
    firstValue = cms.uint64(1),
    lastValue = cms.uint64(1),
    interval = cms.uint64(1)
)


# steer the emulator part
process.hgcalEmulatedSlinkRawData.emulatorType = options.mode
if process.hgcalEmulatedSlinkRawData.emulatorType == 'hgcmodule':
    #process.hgcalEmulatedSlinkRawData.treeName = cms.untracked.string('unpacker_data/hgcroc')
    process.hgcalEmulatedSlinkRawData.inputs = cms.untracked.vstring(options.inputFiles)
process.hgcalEmulatedSlinkRawData.storeEmulatorInfo = bool(options.storeEmulatorInfo)
print(process.hgcalEmulatedSlinkRawData.emulatorType)

# steer the number of capture blocks
if options.randomActiveCaptureBlocks:
    from random import randint
    process.hgcalEmulatedSlinkRawData.slinkParams.numCaptureBlocks = randint(1, 50)  # randomise the number of capture blocks emulated
else:
    process.hgcalEmulatedSlinkRawData.slinkParams.numCaptureBlocks = options.numCaptureBlocks
print('S-link: number of capture blocks: {}'.format(
    process.hgcalEmulatedSlinkRawData.slinkParams.numCaptureBlocks.value()))

# steer the number (and/or list) of ECON-Ds per capture block
if options.numECONDs > 0:
    for i in range(options.numECONDs - len(process.hgcalEmulatedSlinkRawData.slinkParams.ECONDs)):
        process.hgcalEmulatedSlinkRawData.slinkParams.ECONDs.append(process.hgcalEmulatedSlinkRawData.slinkParams.ECONDs[0].clone())
    process.hgcalEmulatedSlinkRawData.slinkParams.checkECONDsLimits = False  # allows to mess with unconventional, high number
                                                                             # of ECON-Ds per capture block

econd_id=0
for econd in process.hgcalEmulatedSlinkRawData.slinkParams.ECONDs:
    # must use 'cms.' python configuration types
    if options.randomActiveECOND:  # randomly turn on/off any ECON-D in capture block
        from random import getrandbits
        econd.active = cms.bool(bool(getrandbits(1)))
    else:  # use a fixed, user-steered list of ECON-Ds in capture block
        econd.active = cms.bool((econd_id in options.activeECONDs))
    econd.passthroughMode = cms.bool((econd_id in options.ECONDsInPassthrough))
    econd.characterisationMode = cms.bool((econd_id in options.ECONDsInCharacterisation))
    econd.enabledERxs = cms.vuint32([i for i in range(options.numERxsPerECOND)])
    econd.numChannelsPerERx = cms.uint32(options.numChannelsPerERx)
    econd.defaultToTStatus = cms.uint32(options.ECONDToTStatus)
    print('ECON-D {}: active? {}, enabled eRxs: {}, number of channels/eRx: {}, passthrough? {}, characterisation? {}'.format(
        econd_id, bool(econd.active),
        [i for i in econd.enabledERxs], econd.numChannelsPerERx.value(),
        bool(econd.passthroughMode), bool(econd.characterisationMode)))
    econd_id += 1

# steer the unpacker
process.hgcalDigis.src = cms.InputTag('hgcalEmulatedSlinkRawData','hgcalFEDRawData')
process.hgcalDigis.fedIds = cms.vuint32(options.fedId)
process.hgcalDigis.maxCaptureBlock = process.hgcalEmulatedSlinkRawData.slinkParams.numCaptureBlocks
process.hgcalDigis.numERxsInECOND = options.numERxsPerECOND
process.hgcalDigis.captureBlockECONDMax = max(  # allows to mess with unconventional, high number of ECON-Ds per capture block
    process.hgcalDigis.captureBlockECONDMax,
    len([ec for ec in process.hgcalEmulatedSlinkRawData.slinkParams.ECONDs if ec.active]))
process.hgcalDigis.config_label = cms.string('') # for HGCalConfigESSourceFromYAML

# TESTERS
process.hgCalSoATester = cms.EDAnalyzer('HGCalSoATester',
                                        Digis = cms.InputTag('hgcalDigis','DIGI'),
                                        SoADigis = cms.InputTag('hgcalDigis',''),)

# Configuration from YAML files
process.hgCalConfigESSourceFromYAML.filename = options.configFile

# RecHit producer: pedestal txt file for DIGI -> RECO calibration
process.hgCalPedestalsESSource.filename = options.pedestalFile

if options.GPU:
    process.hgcalRecHit = cms.EDProducer(
        'alpaka_cuda_async::HGCalRecHitProducer',
        digis = cms.InputTag('hgcalDigis', '', 'TEST'),
        n_hits_scale = cms.int32(50000),
        pedestal_label = cms.string(''), # for HGCalPedestalsESSource
        n_blocks = cms.int32(4096),
        n_threads = cms.int32(1024),
    )
else:
    process.hgcalRecHit = cms.EDProducer(
        'alpaka_serial_sync::HGCalRecHitProducer',
        digis = cms.InputTag('hgcalDigis', '', 'TEST'),
        n_hits_scale = cms.int32(50000),
        pedestal_label = cms.string(''), # for HGCalPedestalsESSource
        n_blocks = cms.int32(1024),
        n_threads = cms.int32(4096),
    )


# DQM
process.tbdqmedanalyzer = cms.EDProducer('HGCalDigisClient',
                                         Digis = cms.InputTag('hgcalDigis','DIGI'),
                                         MetaData = cms.InputTag('hgcalEmulatedSlinkRawData','hgcalMetaData'), )

process.DQMStore = cms.Service("DQMStore")
process.load("DQMServices.FileIO.DQMFileSaverOnline_cfi")
process.dqmSaver.tag = 'HGCAL'
process.dqmSaver.runNumber = 123480

# CONDITIONS, ETC
process.load('Geometry.HGCalMapping.hgCalModuleInfoESSource_cfi')
process.hgCalModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/modulelocator_tb.txt'


# process.p = cms.Path(process.hgcalEmulatedSlinkRawData * process.hgcalDigis * process.tbdqmedanalyzer * process.dqmSaver * process.hgCalSoATester)
process.p = cms.Path(process.hgcalEmulatedSlinkRawData * process.hgcalDigis * process.hgcalRecHit * process.tbdqmedanalyzer * process.dqmSaver * process.hgCalSoATester)

if options.dumpFRD:
    process.dump = cms.EDAnalyzer("DumpFEDRawDataProduct",
        label = cms.untracked.InputTag('hgcalEmulatedSlinkRawData','hgcalFEDRawData'),
        feds = cms.untracked.vint32(options.fedId),
        dumpPayload = cms.untracked.bool(True)
    )
    process.p *= process.dump

process.outpath = cms.EndPath()

if options.storeOutput:
    process.output = cms.OutputModule("PoolOutputModule",
        fileName = cms.untracked.string(options.output),
        outputCommands = cms.untracked.vstring(
            'drop *',
            'keep *_hgcalEmulatedSlinkRawData_*_*',
            'keep *_hgcalDigis_*_*',
            'keep *_hgcalRecHit_*_*',
        )
    )
    process.outpath += process.output

if options.storeRAWOutput:
    process.outputRAW = cms.OutputModule("FRDOutputModule",
        source = cms.InputTag('hgcalEmulatedSlinkRawData'),
        frdVersion = cms.untracked.uint32(6),
        frdFileVersion = cms.untracked.uint32(1),
        fileName = cms.untracked.string(options.secondaryOutput)
    )
    process.outpath += process.outputRAW
