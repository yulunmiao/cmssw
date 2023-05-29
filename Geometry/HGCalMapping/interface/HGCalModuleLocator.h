/****************************************************************************
 *
 * This is a part of HGCal offline software.
 * Authors:
 *   Lovisa Rygaard
 *
 ****************************************************************************/

#ifndef Geometry_HGCalMapping_HGCalModuleLocator_H
#define Geometry_HGCalMapping_HGCalModuleLocator_H

#include "DataFormats/HGCalDigi/interface/HGCalElectronicsId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <algorithm>

class HGCalModuleLocator {

  public:

    HGCalModuleLocator(){};

    void buildLocatorFrom(std::string path,bool usefip=false);

    // HGCalModule from electronics id information (ECON-D idx, Capture Block idx, FED ID)
    HGCalModuleInfo getModule(int econdidx, int captureblockidx, int fedid) const;

    // Module location from electronics id information (ECON-D idx, Capture Block idx, FED ID)
    std::tuple<int,int,int> getModuleLocation(int econdidx, int captureblock, int fedid) const;

    //  Module location from ElectronicsId
    std::tuple<int,int,int> getModuleLocation(HGCalElectronicsId& id) const;

    // ECON-D idx from Module location
    int getEcondIdx(int plane, int u, int v, int isSiPM) const;
    // Capture Block from Module location
    int getCaptureBlockIdx(int plane, int u, int v, int isSiPM) const;
    // FED ID from Module location
    int getFedId(int plane, int u, int v, int isSiPM) const;
    // DAQ from Module location
    std::string getDAQ(int plane, int u, int v, int isSiPM) const;

  private:
  
    //object holding all module info
    HGCalCondSerializableModuleInfo mod2loc_;

};

#endif
