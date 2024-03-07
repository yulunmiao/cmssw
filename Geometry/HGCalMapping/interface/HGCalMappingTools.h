#ifndef _geometry_hgcalmapping_hgcalmappingtools_h_
#define _geometry_hgcalmapping_hgcalmappingtools_h_

#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"

namespace hgcal {

  namespace mappingtools {

    uint16_t getEcondErx(uint16_t chip, uint16_t half);
    uint32_t getElectronicsId(
        bool zside, uint16_t fedid, uint16_t captureblock, uint16_t econdidx, int cellchip, int cellhalf, int cellseq);
    uint32_t getSiDetId(bool zside, int moduleplane, int moduleu, int modulev, int celltype, int celliu, int celliv);
    uint32_t getSiPMDetId(bool zside, int moduleplane, int modulev, int celltype, int celliu, int celliv);

    template<class T1,class T2>
    uint32_t getElectronicsIdForSiCell(const T1 &modules, const T2& cells, uint32_t detid) {

      //get the module and cell parts of the id
      HGCSiliconDetId siid(detid);

      // match module det id
      uint32_t modid = siid.moduleId().rawId();
      for (int i = 0; i<modules.view().metadata().size(); i++) {
        auto imod = modules.view()[i];
        if(imod.detid() != modid) continue;

        //match cell by type of module and by cell det id
        DetId::Detector det(DetId::Detector::HGCalEE);
        uint32_t cellid = 0x3ff & HGCSiliconDetId(det, 0, 0, 0, 0, 0, siid.cellU(), siid.cellV()).rawId();
        for (int j = 0; j < cells.view().metadata().size(); j++) {
          auto jcell = cells.view()[j];
          if (jcell.typeidx() != imod.typeidx()) continue;
          if(jcell.detid() != cellid) continue;
          return imod.eleid() + jcell.eleid();
        }
      }
      
      return 0;
    }
    
  }  // namespace mappingtools
}  // namespace hgcal

#endif
