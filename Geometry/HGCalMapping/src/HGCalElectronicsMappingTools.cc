#include "Geometry/HGCalMapping/interface/HGCalElectronicsMappingTools.h"

namespace hgcal {
  
  std::map<uint32_t,uint32_t> mapSiGeoToElectronics(const HGCalCondSerializableModuleInfo &moduleInfo,
                                                    const HGCalCondSerializableSiCellChannelInfo &siCellInfo,
                                                    bool geo2ele)
  {

    //loop over Si modules
    std::map<uint32_t,uint32_t> idmap;
    for(auto m : moduleInfo.params_){
      
      if(m.isSiPM) continue;
      
      auto isHD=m.isHD;
      auto wafType=m.wafType;

      //loop over cells in this module skipping calibration and unconnected channels
      auto cells=siCellInfo.getAllCellsInModule(isHD,wafType);
      for(auto c : cells) {
        
        if(c.t!=1) continue;

        //build the electronics id
        auto econderx = hgcal::getEcondErxFor(c.chip,c.half);
        uint32_t elecid = HGCalElectronicsId(m.zside,m.fedid,m.captureblock,m.econdidx,econderx,c.seq).raw(); 

        //build the detector id
        DetId::Detector det = m.plane<=26 ? DetId::Detector::HGCalEE : DetId::Detector::HGCalHSi;
        int zp(m.zside ?  1 : -1);
        int celltype(0); //fix me... this is related to thickness...
        uint32_t geoid = HGCSiliconDetId(det,zp,celltype,m.plane,m.u,m.v,c.iu,c.iv).rawId();

        //map
        idmap[geo2ele ? geoid : elecid] = geo2ele ? elecid : geoid;
      }
    }
    
    return idmap;
  }

}
