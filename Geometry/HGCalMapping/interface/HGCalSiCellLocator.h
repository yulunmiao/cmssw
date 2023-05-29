#ifndef _geometry_hgcalmapping_hgcalsicelllocator_h_
#define _geometry_hgcalmapping_hgcalsicelllocator_h_

#include <string>
#include <vector>
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiCellChannelInfo.h"

class HGCalSiCellLocator
{

public:

  HGCalSiCellLocator();
  void buildLocatorFrom(std::string url,bool append=false,bool usefip=false);
  HGCalSiCellChannelInfo locateCellByGeom(int iu,int iv,uint8_t wafType, bool isHD);
  ~HGCalSiCellLocator();

private:

  HGCalCondSerializableSiCellChannelInfo cellColl_;

};



#endif
