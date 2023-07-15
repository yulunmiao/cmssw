import FWCore.ParameterSet.Config as cms


def configTBConditions(process,key='default'):

    """maybe this should be done with eras/modifiers?"""

    process.hgCalModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/modulelocator_tb.txt'
    process.hgCalSiModuleInfoESSource.filename = 'Geometry/HGCalMapping/data/WaferCellMapTraces.txt'

    pedestals={
        'default':'/eos/cms/store/group/dpg_hgcal/comm_hgcal/ykao/calibration_parameters.txt',
    }
    if hasattr(process,'hgCalPedestalsESSource'):
        process.hgCalPedestalsESSource.filename = pedestals[key]

    return process
