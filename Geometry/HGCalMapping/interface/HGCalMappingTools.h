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
      HGCSiliconDetId siid(detid);
      
      //match module by layer, u, v
      for (int i = 0; i<modules.view().metadata().size(); i++) {
        auto imod = modules.view()[i];
        if(!imod.valid()) continue;
        if(imod.zside()!=siid.zside()) continue;
        if(imod.isSiPM()) continue;
        if(imod.plane()!= siid.layer()) continue;
        if(imod.i1()!=siid.waferU()) continue;
        if(imod.i2()!=siid.waferV()) continue;

        //match cell by u,v
        for (int j = 0; j < cells.view().metadata().size(); j++) {
          auto jcell = cells.view()[j];
          if(jcell.typeidx() != imod.typeidx()) continue;
          if(!jcell.valid()) continue;
          if(jcell.i1()!=siid.cellU()) continue;
          if(jcell.i2()!=siid.cellV()) continue;

          return imod.eleid() + jcell.eleid();
        }
      }
      
      return 0;
    }
    
  }  // namespace mappingtools
}  // namespace hgcal

#endif
