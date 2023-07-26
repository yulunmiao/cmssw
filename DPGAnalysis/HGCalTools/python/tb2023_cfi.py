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

def configTBConditions(process,key='default'):

    """ maybe this should be done with eras/modifiers? """

    process.hgCalModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/modulelocator_tb.txt'
    process.hgCalSiModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/WaferCellMapTraces.txt'

    pedestals={
        'default':'/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/calibration_parameters.txt',
    }
    if hasattr(process,'hgCalPedestalsESSource'):
        process.hgCalPedestalsESSource.filename = pedestals[key]
    if hasattr(process,'hgcalCalibrationESProducer'):
        process.hgcalCalibrationESProducer.filename = pedestals[key]

    return process
