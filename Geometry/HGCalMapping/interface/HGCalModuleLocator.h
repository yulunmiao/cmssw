/****************************************************************************
 *
 * This is a part of HGCal offline software.
 * Authors:
 *   Lovisa Rygaard
 *
 ****************************************************************************/

#ifndef Geometry_HGCalMapping_HGCalModuleLocator_H
#define Geometry_HGCalMapping_HGCalModuleLocator_H

#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "CondFormats/HGCalObjects/interface/HGCalCondSerializableModuleInfo.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class HGCalModuleLocator {

  public:

    HGCalModuleLocator(){};

    void buildLocatorFrom(std::string path,bool usefip=false);

    //returns the info
    HGCalCondSerializableModuleInfo getInfo() {return mod2loc_; }
  
  private:
  
    //object holding all module info
    HGCalCondSerializableModuleInfo mod2loc_;

};

#endif
