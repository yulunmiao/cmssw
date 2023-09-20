import FWCore.ParameterSet.Config as cms


def addPerformanceReports(process, addMemCheck=False):

    # add timing and mem (too slow) for FWK jobs report
    process.Timing = cms.Service("Timing",
                                 summaryOnly=cms.untracked.bool(True),
                                 useJobReport=cms.untracked.bool(True))

    if addMemCheck:
        process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
                                                ignoreTotal=cms.untracked.int32(1),
                                                jobReportOutputOnly=cms.untracked.bool(True))

    return process


def configTBConditions_default(process):
    return configTBConditions(process, key='default')


def configTBConditions_MLFL00041(process):
    # cmsRun EventFilter/HGCalRawToDigi/test/tb_raw2reco.py mode=hgcmodule fedId=0 slinkBOE=0x2a cbHeaderMarker=0x0 econdHeaderMarker=0x154 ECONDsInPassthrough=0 activeECONDs=0 inputFiles=/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/BeamTestSep/SingleModuleTest/MLFL00041/position_scan/electron_200GeV/beam_run/run_20230914_153751/beam_run0_roc2root.root output=beam_run0_roc2root conditions=MLFL00041 runNumber=100000 maxEvents=1000000000 ECONDsInCharacterisation=0 numERxsPerECOND=6
    return configTBConditions(process, key='MLFL00041')


def configTBConditions_MLDSL57(process):
    # cmsRun EventFilter/HGCalRawToDigi/test/tb_raw2reco.py mode=hgcmodule fedId=0 slinkBOE=0x2a cbHeaderMarker=0x0 econdHeaderMarker=0x154 ECONDsInPassthrough=0 activeECONDs=0 inputFiles=/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/BeamTestSep/SingleModuleTest/MLDSL57/position_scan/electrons_200GeV_x160_y160/beam_run/run_20230917_132125/beam_run52_roc2root.root output=beam_run52_roc2root conditions=MLDSL57 runNumber=100000 maxEvents=1000000000 ECONDsInCharacterisation=0 numERxsPerECOND=3
    return configTBConditions(process, key='MLDSL57')


def configTBConditions_MLDSR01(process):
    # cmsRun EventFilter/HGCalRawToDigi/test/tb_raw2reco.py mode=hgcmodule fedId=0 slinkBOE=0x2a cbHeaderMarker=0x0 econdHeaderMarker=0x154 ECONDsInPassthrough=0 activeECONDs=0 inputFiles=/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/BeamTestSep/SingleModuleTest/MLDSR01/position_scan/electrons_200GeV_x160_y160/beam_run/run_20230917_131055/beam_run0_roc2root.root output=beam_run0_roc2root.root conditions=MLDSR01 runNumber=100000 maxEvents=1000000000 ECONDsInCharacterisation=0 numERxsPerECOND=3
    return configTBConditions(process, key='MLDSR01')


def configTBConditions_MHFL00056(process):
    # cmsRun EventFilter/HGCalRawToDigi/test/tb_raw2reco.py mode=hgcmodule fedId=0 slinkBOE=0x2a cbHeaderMarker=0x0 econdHeaderMarker=0x154 ECONDsInPassthrough=0 activeECONDs=0 inputFiles=/eos/cms/store/group/dpg_hgcal/tb_hgcal/2023/BeamTestSep/SingleModuleTest/MHFL00056/position_scan/electron_200GeV_x160_y170/beam_run/run_20230915_154222/beam_run0_roc2root.root output=beam_run0_roc2root.root conditions=MHFL00056 runNumber=100000 maxEvents=1000000000 ECONDsInCharacterisation=0 numERxsPerECOND=12
    return configTBConditions(process, key='MHFL00056')


def configTBConditions(process, key='default'):
    """ maybe here we should also set the emulator/unpacker configs in case they are in the process (see comments in the methods above) """

    modulelocator_dict = {
        'default': 'Geometry/HGCalMapping/data/modulelocator_tb.txt',
        'MLFL00041': 'Geometry/HGCalMapping/data/modulelocator_tb_MLFL00041.txt',
        'MLDSL57': 'Geometry/HGCalMapping/data/modulelocator_tb_MLDSL57.txt',
        'MLDSR01': 'Geometry/HGCalMapping/data/modulelocator_tb_MLDSR01.txt',
        'MHFL00056': 'Geometry/HGCalMapping/data/modulelocator_tb_MHFL00056.txt',
    }
    modulelocator = modulelocator_dict[key] if key in modulelocator_dict else modulelocator_dict['default']

    process.hgCalModuleInfoESSource.filename = modulelocator
    process.hgCalSiModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/WaferCellMapTraces.txt'

    pedestals_dict = {
        'default': '/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/calibration_parameters_v2.txt',
    }
    pedestals = pedestals_dict[key] if key in pedestals_dict else pedestals_dict['default']

    if hasattr(process, 'hgCalPedestalsESSource'):
        process.hgCalPedestalsESSource.filename = pedestals
    if hasattr(process, 'hgcalCalibESProducer'):
        process.hgcalCalibESProducer.filename = pedestals
    # if hasattr(process,'hgcalConfigurationESProducer'):
    ###    process.hgcalConfigurationESProducer.filename = yamls[key]

    return process
