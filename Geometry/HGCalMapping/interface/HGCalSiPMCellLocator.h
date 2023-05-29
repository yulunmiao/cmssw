/****************************************************************************
 *
 * This is a part of HGCal offline software.
 * Authors:
 *   Lovisa Rygaard
 *
 ****************************************************************************/

#ifndef Geometry_HGCalMapping_HGCalSiPMCellLocator_H
#define Geometry_HGCalMapping_HGCalSiPMCellLocator_H

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableSiPMTileInfo.h"


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <iterator>
#include <algorithm>

class HGCalSiPMCellLocator {

    public:

        HGCalSiPMCellLocator(){};

        void buildLocatorFrom(std::string channelpath);

        // // Cell location from ROC fields and Module location
        std::tuple<int,int,int> getCellLocation(int seq, int econderx, int halfrocch, int layer, int modiring, int modiphi) const;

        // // Cell location (ring,iphi) from HGCalElectronicsId and Module location
        std::tuple<int,int> getCellLocation(HGCalElectronicsId& id, int seq, int layer, int modiring, int modiphi) const;

        // // DetId from ElectronicsId and Module location, including z-side
        DetId getDetId(HGCalElectronicsId& id, int seq, int z, int layer, int modiring, int modiphi) const;

        // // Module location (layer, ring, iphi) from DetId
        std::tuple<int,int,int> getModuleLocation(DetId& id) const;

    private:
        HGCalCondSerializableSiPMTileInfo cellColl_;

        int getSiPMchannel(int seq, uint8_t econderx, uint8_t halfrocch) const;
};

#endif
