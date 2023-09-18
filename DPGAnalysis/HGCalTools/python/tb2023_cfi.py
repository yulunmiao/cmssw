import FWCore.ParameterSet.Config as cms


def addPerformanceReports(process,addMemCheck=False):

    #add timing and mem (too slow) for FWK jobs report
    process.Timing = cms.Service("Timing",
                                 summaryOnly = cms.untracked.bool(True),
                                 useJobReport = cms.untracked.bool(True))

    
    if addMemCheck:
        process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
                                                ignoreTotal = cms.untracked.int32(1),
                                                jobReportOutputOnly = cms.untracked.bool(True) )
        
    return process

def configTBConditions_default(process):
    return configTBConditions(process,key='default')
    
def configTBConditions_MLFL00041(process):
    return configTBConditions(process,key='MLFL00041')

def configTBConditions(process,key='default'):

    """ maybe this should be done with eras/modifiers? """

    modulelocator_dict = {
        'default':'Geometry/HGCalMapping/data/modulelocator_tb.txt',
        'MLFL00041':'Geometry/HGCalMapping/data/modulelocator_tb_MLFL00041.txt',
    }
    modulelocator = modulelocator_dict[key] if key in modulelocator_dict else modulelocator_dict['default']
    
    process.hgCalModuleInfoESSource.filename = modulelocator
    process.hgCalSiModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/WaferCellMapTraces.txt'
    
    pedestals_dict = {
        'default': '/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/calibration_parameters_v2.txt',
        'MLFL00041':'/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/calibration_parameters_v2.txt'
    }
    pedestals = pedestals_dict[key] if key in pedestals_dict else pedestals_dict['default']
    
    if hasattr(process,'hgCalPedestalsESSource'):
        process.hgCalPedestalsESSource.filename = pedestals
    if hasattr(process,'hgcalCalibESProducer'):
        process.hgcalCalibESProducer.filename = pedestals
    ###if hasattr(process,'hgcalConfigurationESProducer'):
    ###    process.hgcalConfigurationESProducer.filename = yamls[key]
    
    return process
